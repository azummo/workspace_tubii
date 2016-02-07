#ifndef LOGGING_H
#define LOGGING_H

/* Log levels */
#define DEBUG 0
#define VERBOSE 1
#define NOTICE 2
#define WARNING 3
#define LOG_RAW (1<<10) /* Modifier to log without timestamp */
#define DEFAULT_VERBOSITY NOTICE

extern char logfile[256];
extern int verbosity;
extern int syslog_enabled;

void LogRaw(int level, const char *msg);
void Log(int level, const char *fmt, ...);

#endif
