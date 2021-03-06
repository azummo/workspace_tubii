#include <libpq-fe.h>
#include <time.h>
#include "ae.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "logging.h"
#include "db.h"
#include "sds.h"

#define DB_CHECK_QUEUE_DELAY 100

static void db_write(aeEventLoop *el, int fd, void *data, int mask);
static void db_read(aeEventLoop *el, int fd, void *data, int mask);
static int db_connect_event(aeEventLoop *el, long long id, void *data);
static void pop_request(database *db);
static void send_to_db(database *db, const char *command);

static void db_connect_poll(aeEventLoop *el, int fd, void *data, int mask)
{
    database *db = (database *) data;

    int status = PQconnectPoll(db->conn);

    switch (status) {
    case PGRES_POLLING_WRITING:
        if (aeCreateFileEvent(el, fd, AE_WRITABLE, db_connect_poll,
                              db) == AE_ERR) {
            Log(WARNING, "failed to create database connect poll event.");
            goto err;
        }
        break;
    case PGRES_POLLING_READING:
        if (aeCreateFileEvent(el, fd, AE_READABLE, db_connect_poll,
                              db) == AE_ERR) {
            Log(WARNING, "failed to create database connect poll event.");
            goto err;
        }
        break;
    case PGRES_POLLING_OK:
        Log(NOTICE, "connected to database at %s", db->host);

        if (PQsetnonblocking(db->conn, 1)) {
            Log(WARNING, "PQsetnonblocking");
        }

        aeDeleteFileEvent(el, fd, AE_WRITABLE);
        aeDeleteFileEvent(el, fd, AE_READABLE);

        if (aeCreateFileEvent(el, fd, AE_READABLE, db_read, db) == AE_ERR) {
            Log(WARNING, "failed to set up database read event.");
            goto err;
        }

        db->connected = 1;

        break;
    case PGRES_POLLING_FAILED:
        Log(WARNING, "failed to connect to database at %s", db->host);
        goto err;
    default:
        Log(WARNING, "unknown return value %i from PQconnectPoll", status);
        goto err;
    }

    return;

err:
    PQfinish(db->conn);
    db->conn = NULL;
    db->connected = 0;
    aeDeleteFileEvent(el, fd, AE_WRITABLE);
    aeDeleteFileEvent(el, fd, AE_READABLE);

    /* Try to connect again in 10 seconds. */
    if (aeCreateTimeEvent(el, 10000, db_connect_event, db, NULL) == AE_ERR) {
        Log(WARNING, "failed to create db connect event");
    }
}

static void db_get_result(database *db)
{
    PGresult *res, *last_result;

    if (!db->request_list) {
        Log(WARNING, "Got result from database, but no active request!");
        return;
    }

    last_result = PQgetResult(db->conn);

    while ((res = PQgetResult(db->conn))) {
        PQclear(last_result);

        last_result = res;
    }

    /* Call the callback with the last result. */
    if (db->request_list->callback) {
        db->request_list->callback(last_result, db->conn, db->request_list->data);
    }

    PQclear(last_result);

    pop_request(db);
}

static void clear_db_requests(database *db)
{
    dbRequest *req;

    while ((req = db->request_list)) {
        /* Call request callbacks with NULL. */
        if (req->callback) {
            req->callback(NULL, db->conn, req->data);
        }
        db->request_list = req->next;
        free(req->command);
        free(req);
    }
}

static void db_read(aeEventLoop *el, int fd, void *data, int mask)
{
    database *db = (database *) data;

    if (PQconsumeInput(db->conn) != 1) {
        Log(WARNING, "PQconsumeInput: %s", PQerrorMessage(db->conn));
        goto err;
    }

    if (PQisBusy(db->conn) == 0) {
        /* A result is ready. */
        db_get_result(db);
    }

    return;

err:
    clear_db_requests(db);
    PQfinish(db->conn);
    db->conn = NULL;
    db->connected = 0;
    aeDeleteFileEvent(el, fd, AE_WRITABLE);
    aeDeleteFileEvent(el, fd, AE_READABLE);

    /* Try to connect again in 10 seconds. */
    if (aeCreateTimeEvent(el, 10000, db_connect_event, db, NULL) == AE_ERR) {
        Log(WARNING, "failed to create db connect event");
    }
}

static void db_write(aeEventLoop *el, int fd, void *data, int mask)
{
    database *db = (database *) data;

    if (PQflush(db->conn) == 0) {
        /* Successfully flushed or the queue is empty. */
        aeDeleteFileEvent(el, fd, AE_WRITABLE);
        return;
    }

    if (PQstatus(db->conn) == CONNECTION_BAD) {
        Log(WARNING, "database connection disconnected");
        goto err;
    }

    return;

err:
    clear_db_requests(db);
    PQfinish(db->conn);
    db->conn = NULL;
    db->connected = 0;
    aeDeleteFileEvent(el, fd, AE_WRITABLE);
    aeDeleteFileEvent(el, fd, AE_READABLE);

    /* Try to connect again in 10 seconds. */
    if (aeCreateTimeEvent(el, 10000, db_connect_event, db, NULL) == AE_ERR) {
        Log(WARNING, "failed to create db connect event");
    }
}

static void pop_request(database *db)
{
    /* Pop a request off the head of the linked list,
     * and send the next request. */

    if (db->request_list == NULL) {
        Log(WARNING, "attempt to pop NULL request");
        return;
    }

    dbRequest *req = db->request_list;

    if (req->next) {
        db->request_list = req->next;

        time(&db->request_list->time_sent);

        send_to_db(db, db->request_list->command);
    } else {
        db->request_list = NULL;
    }
    free(req->command);
    free(req);
}

static int db_check_queue(struct aeEventLoop *el, long long id, void *data)
{
    time_t now = time(NULL);
    database *db = (database *) data;

    if (db->request_list) {
        if (now > db->request_list->time_sent + db->request_list->timeout) {
            Log(WARNING, "database query timed out");

            /* Since the only way to match up queries with responses is the
             * order they come in, we basically have to disconnect here and
             * then reconnect. */
            int fd = PQsocket(db->conn);

            clear_db_requests(db);
            PQfinish(db->conn);
            db->conn = NULL;
            db->connected = 0;

            if (fd > 0) {
                aeDeleteFileEvent(el, fd, AE_WRITABLE);
                aeDeleteFileEvent(el, fd, AE_READABLE);
            }

            /* Try to connect again immediately. */
            if (aeCreateTimeEvent(el, 0, db_connect_event, db, NULL) == AE_ERR) {
                Log(WARNING, "failed to create db connect event");
            }
        }
    }

    return DB_CHECK_QUEUE_DELAY;
}

void clear_db_requests_from_client(database *db, void *c)
{
    /* Clear all requests whose data pointer points to c. */
    dbRequest *req, *last, *next;

    req = db->request_list;

    /* Delete requests from this client, except for the current request.
     * If the current request is from this client,
     * set request->client = NULL. */
    if (req) {
        if (req->data == c) req->data = NULL;

        last = req;

        req = req->next;

        while (req) {
            next = req->next;

            if (req->data == c) {
                last->next = req->next;

                free(req->command);
                free(req);
            } else {
                last = req;
            }

            req = next;
        }
    }
}

int db_exec_async(database *db, const char *command, dbCallback *callback, void *data)
{
    /* Function to asynchronously execute a query to the database. The function
     * signature for callback should be:
     *
     *     void callback_function(PGresult *res, PGconn *conn, void *data);
     *
     * and will be called with the result when the query is completed. If there
     * was a problem, it will be called with res = NULL.
     * 
     * Returns 0 on success or -1 if the database is not connected. */
	if (db->connected == 0) return -1;

    dbRequest *req = (dbRequest *) malloc(sizeof(dbRequest));
    req->command = (char *) malloc(strlen(command)+1);
    strcpy(req->command, command);
    req->callback = callback;
    req->next = NULL;
    req->data = data;
    req->timeout = 1;

    if (db->request_list == NULL) {
        /* There are no active database requests, so just go ahead
         * and send the request. */
        db->request_list = req;
        /* send request */
        time(&req->time_sent);

        send_to_db(db, req->command);
    } else {
    	dbRequest *tail = db->request_list;
        /* Request goes on the end of the queue. */
        while (tail->next != NULL) tail = tail->next;
        tail->next = req;
    }

    return 0;
}

static void send_to_db(database *db, const char *command)
{
    PQsendQuery(db->conn, command);

    int sock = PQsocket(db->conn);

    if (aeCreateFileEvent(db->el, sock, AE_WRITABLE, db_write, db) == AE_ERR) {
        Log(WARNING, "failed to create database write event.");
        goto err;
    }

    return;

err:
    clear_db_requests(db);
    PQfinish(db->conn);
    db->conn = NULL;
    db->connected = 0;
    aeDeleteFileEvent(db->el, sock, AE_WRITABLE);
    aeDeleteFileEvent(db->el, sock, AE_READABLE);

    /* Try to connect again in 10 seconds. */
    if (aeCreateTimeEvent(db->el, 10000, db_connect_event, db, NULL) == AE_ERR) {
        Log(WARNING, "failed to create db connect event");
    }
}

static int db_connect_event(aeEventLoop *el, long long id, void *data)
{
    char conninfo[1024];

    database *db = (database *) data;

    sprintf(conninfo, "dbname=%s host=%s user=%s password=%s",
            db->name, db->host, db->user, db->pass);

    /* Try to connect to the database. If it fails, try again 10 seconds
     * later */
    db->conn = PQconnectStart(conninfo);

    if (db->conn == NULL) {
        Log(WARNING, "failed to connect to database.");
        return 10000;
    }

    if (PQstatus(db->conn) == CONNECTION_BAD) {
        Log(WARNING, "failed to connect to database.");
        /* If PQconnectStart returns a non NULL pointer, we have to call
         * PQfinish.
         * See www.postgresql.org/docs/9.1/tstatic/libpq-connect.html. */
        PQfinish(db->conn);
        db->conn = NULL;
        return 10000;
    }

    int sock = PQsocket(db->conn);

    /* Now we have successfully started the non blocking connect. The next
     * procedure is outlined in the documentation. We wait until the socket
     * is writable, then call PQconnectPoll(). */
    if (aeCreateFileEvent(el, sock, AE_WRITABLE, db_connect_poll,
                          db) == AE_ERR) {
        Log(WARNING, "failed to create database connect poll event");
        PQfinish(db->conn);
        db->conn = NULL;
        return 10000;
    }

    return AE_NOMORE;
}

void free_database(database *db)
{
    clear_db_requests(db);

    if (db->conn) {
        int sock = PQsocket(db->conn);

        aeDeleteFileEvent(db->el, sock, AE_WRITABLE);
        aeDeleteFileEvent(db->el, sock, AE_READABLE);

        PQfinish(db->conn);
        db->conn = NULL;
    }

    sdsfree(db->host);
    sdsfree(db->name);
    sdsfree(db->user);
    sdsfree(db->pass);

    if (db->check_queue_id != AE_DELETED_EVENT_ID) {
        aeDeleteTimeEvent(db->el, db->check_queue_id);
    }

    free(db);
}

database *db_connect(aeEventLoop *el, const char *host, const char *name, const char *user, const char *pass)
{
    /* Connect asynchronously to a postgres database. Returns the database
     * pointer you can use to execute asynchronous queries. For example:
     *
     *     database *db = db_connect(el, host, name, user, pass);
     *
     *     if (db = NULL) {
     *         Log(WARNING, "failed to connect to database at %s", host);
     *     }
     *
     *     db_exec_async(db, "select * from foo where bar = 'baz'", callback, NULL);
     */
    database *db = (database *) malloc(sizeof(database));
    db->el = el;
    db->host = sdsnew(host);
    db->name = sdsnew(name);
    db->user = sdsnew(user);
    db->pass = sdsnew(pass);
    db->conn = NULL;
    db->request_list = NULL;
    db->connected = 0;
    db->check_queue_id = AE_DELETED_EVENT_ID;

    if (aeCreateTimeEvent(el, 0, db_connect_event, db, NULL) == AE_ERR) {
        Log(WARNING, "failed to create db connect event");
        free_database(db);
        return NULL;
    }

    if ((db->check_queue_id = aeCreateTimeEvent(el, 100, db_check_queue, db, NULL)) == AE_ERR) {
        Log(WARNING, "failed to set up check queue timed event");
        free_database(db);
    }

    return db;
}

