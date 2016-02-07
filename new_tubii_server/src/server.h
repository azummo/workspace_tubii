#ifndef SERVER_H
#define SERVER_H

#include "sds.h"
#include <time.h>
#include "adlist.h"
#include "dict.h"
#include "ae.h"
#include "anet.h"

typedef long long mstime_t; /* millisecond time type. */

typedef void logFunction(int level, const char *fmt, ...);

/* Error codes */
#define C_OK                    0
#define C_ERR                   -1

/* Static server configuration */
#define CONFIG_DEFAULT_HZ        10      /* Time interrupt calls/sec. */
#define CONFIG_DEFAULT_SERVER_PORT        4000    /* TCP port */
#define CONFIG_DEFAULT_TCP_BACKLOG       511     /* TCP listen backlog */
#define CONFIG_BINDADDR_MAX 16
#define NET_IP_STR_LEN 46 /* INET6_ADDRSTRLEN is 46, but we need to be sure */
#define CONFIG_DEFAULT_TCP_KEEPALIVE 0
#define CONFIG_DEFAULT_MAX_CLIENTS 10000

/* Protocol and I/O related defines */
#define PROTO_MAX_QUERYBUF_LEN  (1024*1024*1024) /* 1GB max query buffer. */
#define PROTO_IOBUF_LEN         (1024*16)  /* Generic I/O buffer size */
#define PROTO_REPLY_CHUNK_BYTES (16*1024) /* 16k output buffer */
#define PROTO_INLINE_MAX_SIZE   (1024*64) /* Max size of inline reads */
#define PROTO_MBULK_BIG_ARG     (1024*32)
#define LONG_STR_SIZE      21          /* Bytes needed for long -> str */
#define AOF_AUTOSYNC_BYTES (1024*1024*32) /* fdatasync every 32MB */

/* Client flags */
#define CLIENT_SLAVE (1<<0)   /* This client is a slave server */
#define CLIENT_MASTER (1<<1)  /* This client is a master server */
#define CLIENT_MONITOR (1<<2) /* This client is a slave monitor, see MONITOR */
#define CLIENT_MULTI (1<<3)   /* This client is in a MULTI context */
#define CLIENT_BLOCKED (1<<4) /* The client is waiting in a blocking operation */
#define CLIENT_DIRTY_CAS (1<<5) /* Watched keys modified. EXEC will fail. */
#define CLIENT_CLOSE_AFTER_REPLY (1<<6) /* Close after writing entire reply. */
#define CLIENT_UNBLOCKED (1<<7) /* This client was unblocked and is stored in
                                  server.unblocked_clients */
#define CLIENT_LUA (1<<8) /* This is a non connected client used by Lua */
#define CLIENT_ASKING (1<<9)     /* Client issued the ASKING command */
#define CLIENT_CLOSE_ASAP (1<<10)/* Close this client ASAP */
#define CLIENT_UNIX_SOCKET (1<<11) /* Client connected via Unix domain socket */
#define CLIENT_DIRTY_EXEC (1<<12)  /* EXEC will fail for errors while queueing */
#define CLIENT_MASTER_FORCE_REPLY (1<<13)  /* Queue replies even if is master */
#define CLIENT_FORCE_AOF (1<<14)   /* Force AOF propagation of current cmd. */
#define CLIENT_FORCE_REPL (1<<15)  /* Force replication of current cmd. */
#define CLIENT_PRE_PSYNC (1<<16)   /* Instance don't understand PSYNC. */
#define CLIENT_READONLY (1<<17)    /* Cluster client is in read-only state. */
#define CLIENT_PUBSUB (1<<18)      /* Client is in Pub/Sub mode. */
#define CLIENT_PREVENT_PROP (1<<19)  /* Don't propagate to AOF / Slaves. */

/* Client request types */
#define PROTO_REQ_INLINE 1
#define PROTO_REQ_MULTIBULK 2
/* Log levels */
#define LL_DEBUG 0
#define LL_VERBOSE 1
#define LL_NOTICE 2
#define LL_WARNING 3
#define LL_RAW (1<<10) /* Modifier to log without timestamp */
#define CONFIG_DEFAULT_VERBOSITY LL_NOTICE

/* Anti-warning macro... */
#define UNUSED(V) ((void) V)

/* We can print the stacktrace, so our assert is defined this way: */
/*
#define serverAssertWithInfo(_c,_o,_e) ((_e)?(void)0 : (_serverAssertWithInfo(_c,_o,#_e,__FILE__,__LINE__),_exit(1)))
#define serverAssert(_e) ((_e)?(void)0 : (_serverAssert(#_e,__FILE__,__LINE__),_exit(1)))
#define serverPanic(_e) _serverPanic(#_e,__FILE__,__LINE__),_exit(1)
*/
#include <stdio.h>
#define serverAssertWithInfo(_c,_o,_e) ((_e)?(void)0 : (fprintf(stderr, "blah\n"),exit(1)))
#define serverAssert(_e) ((_e)?(void)0 : (fprintf(stderr, "blah %s: %d\n", __FILE__, __LINE__),exit(1)))
#define serverPanic(_e) exit(1)

typedef struct client {
    uint64_t id;            /* Client incremental unique ID. */
    int fd;                 /* Client socket. */
    sds name;               /* for logging server */
    sds querybuf;           /* Buffer we use to accumulate client queries. */
    size_t querybuf_peak;   /* Recent (100ms or more) peak of querybuf size. */
    int argc;               /* Num of arguments of current command. */
    sds *argv;              /* Arguments of current command. */
    struct command *cmd, *lastcmd;  /* Last command executed. */
    int reqtype;            /* Request protocol type: PROTO_REQ_* */
    int multibulklen;       /* Number of multi bulk arguments left to read. */
    long bulklen;           /* Length of bulk argument in multi bulk request. */
    size_t sentlen;         /* Amount of bytes already sent in the current
                               buffer or object being sent. */
    time_t ctime;           /* Client creation time. */
    time_t lastinteraction; /* Time of the last interaction, used for timeout */
    int flags;              /* Client flags: CLIENT_* macros. */

    /* Response buffer */
    int bufpos;
    char buf[PROTO_REPLY_CHUNK_BYTES];
} client;

/* command functions should have this signature */
typedef void command_func(client *c, int argc, sds *argv);

struct command {
    char *name;
    command_func *func;
    int arity;
};

struct redisServer {
    /* General */
    int hz;                     /* serverCron() calls frequency in hertz */
    dict *commands;             /* Command table */
    aeEventLoop *el;
    int cronloops;              /* Number of times the cron function run */
    logFunction *log;
    /* Networking */
    int port;                   /* TCP listening port */
    int tcp_backlog;            /* TCP listen() backlog */
    char *bindaddr[CONFIG_BINDADDR_MAX]; /* Addresses we should bind to */
    int bindaddr_count;         /* Number of addresses in server.bindaddr[] */
    int ipfd[CONFIG_BINDADDR_MAX]; /* TCP socket file descriptors */
    int ipfd_count;             /* Used slots in ipfd[] */
    list *clients;              /* List of active clients */
    list *clients_to_close;     /* Clients to close asynchronously */
    client *current_client; /* Current client, only used on crash report */
    char neterr[ANET_ERR_LEN];   /* Error buffer for anet.c */
    uint64_t next_client_id;    /* Next client unique ID. Incremental. */
    /* Fields used only for stats */
    time_t stat_starttime;          /* Server start time */
    long long stat_numcommands;     /* Number of processed commands */
    long long stat_numconnections;  /* Number of connections received */
    long long stat_rejected_conn;   /* Clients rejected because of maxclients */
    long long stat_net_input_bytes; /* Bytes read from network. */
    long long stat_net_output_bytes; /* Bytes written to network. */
    /* Configuration */
    int verbosity;                  /* Loglevel in redis.conf */
    int tcpkeepalive;               /* Set SO_KEEPALIVE if non-zero. */
    size_t client_max_querybuf_len; /* Limit for client query buffer length */
    /* Limits */
    unsigned int maxclients;            /* Max number of simultaneous clients */
    /* time cache */
    time_t unixtime;        /* Unix time sampled every cron cycle. */
    long long mstime;       /* Like 'unixtime' but with milliseconds resolution. */
};

void setServerLog(logFunction *log);

unsigned int dictSdsHash(const void *key);
unsigned int dictSdsCaseHash(const void *key);
long long ustime(void);
mstime_t mstime(void);
client *createClient(int fd);
int prepareClientToWrite(client *c);
int _addReplyToBuffer(client *c, const char *s, size_t len);
void addReplyString(client *c, const char *s, size_t len);
void addReply(client *c, const char *fmt, ...);
void addReplyErrorLength(client *c, const char *s, size_t len);
void addReplyError(client *c, const char *err);
void addReplyErrorFormat(client *c, const char *fmt, ...);
void addReplyStatusLength(client *c, const char *s, size_t len);
void addReplyStatus(client *c, const char *status);
void addReplyStatusFormat(client *c, const char *fmt, ...);
void acceptTcpHandler(aeEventLoop *el, int fd, void *privdata, int mask);
void freeClient(client *c);
void freeClientAsync(client *c);
void freeClientsInAsyncFreeQueue(void);
void sendReplyToClient(aeEventLoop *el, int fd, void *privdata, int mask);
void resetClient(client *c);
int processInlineBuffer(client *c);
int processMultibulkBuffer(client *c);
void processInputBuffer(client *c);
void readQueryFromClient(aeEventLoop *el, int fd, void *privdata, int mask);
sds catClientInfoString(sds s, client *client);
sds getAllClientsInfoString(void);
void updateCachedTime(void);
int serverCron(struct aeEventLoop *eventLoop, long long id, void *clientData);
int listenToPort(int port, int *fds, int *count);
void resetServerStats(void);
void initServerConfig(void);
void initServer(aeEventLoop *el, int port, struct command *commandTable, int numcommands);
struct command *lookupCommand(sds name);
struct command *lookupCommandByCString(char *s);
int processCommand(client *c);
#endif
