#include "logging.h"
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <syslog.h>
#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_LOGMSG_LEN 1024

char logfile[256] = "\0";
int verbosity = 1;
int syslog_enabled = 0;

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

/* Like redisLogRaw() but with printf-alike support. This is the function that
 * is used across the code. The raw version is only used in order to dump
 * the INFO output on crash. */
void Log(int level, const char *fmt, ...) {
    va_list ap;
    char msg[MAX_LOGMSG_LEN];

    if ((level&0xff) < verbosity) return;

    va_start(ap, fmt);
    vsnprintf(msg, sizeof(msg), fmt, ap);
    va_end(ap);

    LogRaw(level,msg);
}

