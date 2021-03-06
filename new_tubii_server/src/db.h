#ifndef DB_H
#define DB_H

#include <libpq-fe.h>
#include <time.h>
#include "sds.h"
#include "ae.h"

/* Function signature for the callback called whenever a request gets
 * a result from the database. If the request fails, result will be set
 * to NULL.
 *
 * Note that this callback will only be called with the *last* SQL statement
 * in the query in order to mimic the result of PQexec(), i.e. if you send
 * a query like this:
 *
 *     db_exec_async("select 42; select "Hello", callback, NULL);
 *
 * Then the callback will be called *only* with the result of the second
 * statement.
 *
 * See http://postgresql.nabble.com/libpq-PQsendQuery-wait-for-complete-result-td5734111.html#a5734321. for more details. */
typedef void dbCallback(PGresult *res, PGconn *conn, void *data);

typedef struct dbRequest {
    char *command;
    time_t time_sent;
    int timeout;
    dbCallback *callback;
    void *data;
    struct dbRequest *next;
} dbRequest;

typedef struct database {
    aeEventLoop *el;

    sds host;
    sds name;
    sds user;
    sds pass;
    PGconn *conn;

    long long check_queue_id;

    int connected;

    dbRequest *request_list;
} database;

database *db_connect(aeEventLoop *el, const char *host, const char *name, const char *user, const char *pass);
int db_exec_async(database *db, const char *command, dbCallback *callback, void *data);
void clear_db_requests_from_client(database *db, void *c);

#endif
