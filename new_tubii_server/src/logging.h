#ifndef LOGGING_H
#define LOGGING_H

#include "ae.h"

/* Log levels */
#define DEBUG 0
#define VERBOSE 1
#define NOTICE 2
#define WARNING 3
#define LOG_RAW (1<<10) /* Modifier to log without timestamp */
#define DEFAULT_VERBOSITY NOTICE

#define LOG_SERVER_PORT 4001
#define LOG_SERVER_RECONNECT 10000

extern char logfile[256];
extern int verbosity;
extern int syslog_enabled;

/* Function signature for a low level logging function. This allows programs
 * to customize the logging messages used by the rest of the code by creating
 * a function with this signature and then calling setLogger(). */
typedef void loggingFunction(int level, const char *msg);

void startLogServer(char *host, char *name);

int printSkipped(aeEventLoop *el, long long id, void *_);

void setLogger(loggingFunction *f);
void LogRawName(int level, const char *msg, const char *name);
void LogRaw(int level, const char *msg);
void Log(int level, const char *fmt, ...);

#endif
