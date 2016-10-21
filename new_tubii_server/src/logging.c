#include "logging.h"
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <syslog.h>
#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include "hiredis.h"
#include "async.h"
#include "adapters_ae.h"
#include "read.h"

#define MAX_LOGMSG_LEN 2048
#define LOG_SKIPPED_CHECK 10
#define MAX_LOGS 100

char logfile[256] = "\0";
int verbosity = 1;
int syslog_enabled = 0;

/* set to 1 if the log server connection was successful */
static int connected = 0;

redisAsyncContext *log_server;

extern aeEventLoop *el;

loggingFunction *logger = LogRaw;

static void connectCallback(redisAsyncContext *c, int status);
static void disconnectCallback(const redisAsyncContext *c, int status);
static int connectLogServer(aeEventLoop *el, long long id, void *privdata);
static int ping(struct aeEventLoop *el, long long id, void *clientData);

/* log server hostname */
char loghost[256];
/* program name sent to log server */
char logname[256];

static void logCallback(redisAsyncContext *c, void *r, void *privdata)
{
    redisReply *reply = r;

    if (reply == NULL) {
        Log(WARNING, "reply from log server is NULL");
        return;
    }

    if (reply->type == REDIS_REPLY_ERROR) {
        Log(WARNING, reply->str);
    }
}

void startLogServer(char *host, char *name)
{
    strcpy(loghost, host);
    strcpy(logname, name);

    /* try to immediately connect to the log server */
    if (aeCreateTimeEvent(el, 0, connectLogServer, NULL, NULL) == AE_ERR) {
        Log(WARNING, "failed to set up log server connect event.");
        el->stop = 1;
    }

    /* try to immediately ping the log server */
    if (aeCreateTimeEvent(el, 0, ping, NULL, NULL) == AE_ERR) {
        Log(WARNING, "failed to set up ping event.");
        el->stop = 1;
    }
}

static int ping(struct aeEventLoop *el, long long id, void *clientData)
{
    if (connected)
        redisAsyncCommand(log_server, logCallback, NULL, "ping");

    return 1000;
}

static void connectCallback(redisAsyncContext *c, int status)
{
    if (status != REDIS_OK) {
        Log(WARNING, "failed to connect to log server");
        if (aeCreateTimeEvent(el, LOG_SERVER_RECONNECT, connectLogServer, NULL, NULL) == AE_ERR) {
            Log(WARNING, "failed to set up log server connect event.");
            el->stop = 1;
        }
        return;
    }

    /* send name */
    redisAsyncCommand(c, NULL, NULL, "name %s", logname);

    connected = 1;

    Log(NOTICE, "connected to log server");
}

static void disconnectCallback(const redisAsyncContext *c, int status)
{
    /* make sure we don't try to log statements since we are disconnected */
    connected = 0;

    if (status != REDIS_OK) {
        Log(WARNING, "disconnected from log server: %s", c->errstr);

        if (aeCreateTimeEvent(el, LOG_SERVER_RECONNECT, connectLogServer, NULL, NULL) == AE_ERR) {
            Log(WARNING, "failed to set up log server connect event.");
            el->stop = 1;
            return;
        }

        return;
    }

    Log(NOTICE, "disconnected from log server.");
}

static int connectLogServer(aeEventLoop *el, long long id, void *privdata)
{
    log_server = redisAsyncConnect(loghost, LOG_SERVER_PORT);
    if (log_server->err) {
        Log(WARNING, "connectLogServer: %s", log_server->errstr);
        redisAsyncDisconnect(log_server);
        return LOG_SERVER_RECONNECT;
    }

    redisAeAttach(el, log_server);
    redisAsyncSetConnectCallback(log_server, connectCallback);
    redisAsyncSetDisconnectCallback(log_server, disconnectCallback);

    return AE_NOMORE;
}

/* Low level logging. To use only for very big messages, otherwise
 * Log() is to prefer. */
void LogRawName(int level, const char *msg, const char *name) {
    const int syslogLevelMap[] = { LOG_DEBUG, LOG_INFO, LOG_NOTICE, LOG_WARNING };
    const char *c = ".-*#";
    FILE *fp;
    char buf[64];
    int rawmode = (level & LOG_RAW);
    int log_to_stdout = logfile[0] == '\0';

    level &= 0xff; /* clear flags */
    if (level < verbosity) return;

    fp = log_to_stdout ? stdout : fopen(logfile,"a");
    if (!fp) return;

    if (rawmode) {
        fprintf(fp,"%s",msg);
    } else {
        struct timeval tv;

        gettimeofday(&tv,NULL);
        strftime(buf,sizeof(buf),"%d %b %H:%M:%S",localtime(&tv.tv_sec));
        fprintf(fp,"%s %4s %c %s\n", buf, name, c[level],msg);
    }
    fflush(fp);

    if (!log_to_stdout) fclose(fp);
    if (syslog_enabled) syslog(syslogLevelMap[level], "%s", msg);
}

/* Low level logging. To use only for very big messages, otherwise
 * Log() is to prefer. */
void LogRaw(int level, const char *msg) {
    const int syslogLevelMap[] = { LOG_DEBUG, LOG_INFO, LOG_NOTICE, LOG_WARNING };
    const char *c = ".-*#";
    FILE *fp;
    char buf[64];
    int rawmode = (level & LOG_RAW);
    int log_to_stdout = logfile[0] == '\0';

    level &= 0xff; /* clear flags */
    if (level < verbosity) return;

    fp = log_to_stdout ? stdout : fopen(logfile,"a");
    if (!fp) return;

    if (rawmode) {
        fprintf(fp,"%s",msg);
    } else {
        int off;
        struct timeval tv;
        pid_t pid = getpid();

        gettimeofday(&tv,NULL);
        off = strftime(buf,sizeof(buf),"%d %b %H:%M:%S.",localtime(&tv.tv_sec));
        snprintf(buf+off,sizeof(buf)-off,"%03d",(int)tv.tv_usec/1000);
        fprintf(fp,"%d:%s %c %s\n", (int)pid,buf,c[level],msg);
    }
    fflush(fp);

    if (!log_to_stdout) fclose(fp);
    if (syslog_enabled) syslog(syslogLevelMap[level], "%s", msg);
}

int printSkipped(aeEventLoop *el, long long id, void *_)
{
    /* this is a hack so that the Log() function is called once
     * a second so that it prints skipped messages. */
    Log(DEBUG, NULL);
    return 1000;
}

void setLogger(loggingFunction *f)
{
    logger = f;
}

/* Like redisLogRaw() but with printf-alike support. This is the function that
 * is used across the code. The raw version is only used in order to dump
 * the INFO output on crash. */
void Log(int level, const char *fmt, ...) {
    va_list ap;
    char msg[MAX_LOGMSG_LEN];
    static time_t last_time;
    static int count;

    time_t now = time(NULL);

    if (now > last_time + LOG_SKIPPED_CHECK) {
        last_time = now;

        if (count > MAX_LOGS) {
            char skipped[MAX_LOGMSG_LEN];
            sprintf(skipped, "[ skipped %d messages ]", count-10);
            logger(WARNING,skipped);
            if (connected) redisAsyncCommand(log_server, logCallback, NULL, "log %i %s", WARNING, skipped);
        }

        count = 0;
    }

    if ((level&0xff) < verbosity) return;

    if (fmt == NULL) return; /* for printSkipped() */

    count += 1;

    if (count > MAX_LOGS) {
        if (count == MAX_LOGS + 1) {
            logger(WARNING, "[ skipping messages... ]");
            if (connected) redisAsyncCommand(log_server, logCallback, NULL, "log %i %s", WARNING, "[ skipping messages... ]");
        }
        return;
    }

    va_start(ap, fmt);
    vsnprintf(msg, sizeof(msg), fmt, ap);
    va_end(ap);

    logger(level,msg);

    if (connected) {
        redisAsyncCommand(log_server, logCallback, NULL, "log %i %s", level, msg);
    }
}

