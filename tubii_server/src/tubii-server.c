#include "tubii_client.h"
#include "ae.h"
#include "anet.h"
#include <errno.h>
#include <stdio.h>
#include "data.h"
#include <signal.h>
#include "logging.h"
#include "sys/mman.h"
#include <fcntl.h>

aeEventLoop *el;

void sigint_handler(int dummy)
{
    el->stop = 1;
}

#define IP_STR_LEN 46
#define BUFSIZE 256

void client_listen(aeEventLoop *el, int fd, void *data, int mask)
{
    int port;
    char ip[IP_STR_LEN];
    char err[ANET_ERR_LEN];
    int cfd = anetTcpAccept(err, fd, ip, IP_STR_LEN, &port);

    if (cfd == ANET_ERR) {
        if (errno != EWOULDBLOCK) Log(WARNING, "anetTcpAccept: %s", err);
        return;
    }

    Log(NOTICE, "client connected from %s on port %d", ip, port);

    init_client(cfd, BUFSIZE);
}
        
int main(int argc, char **argv)
{
    char err[ANET_ERR_LEN];

    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, sigint_handler);

    Log(NOTICE, "mtc server started");

    el = aeCreateEventLoop(100);

    int listenfd = anetTcpServer(err, 4001, NULL, 10);

    if (listenfd == ANET_ERR) {
        Log(WARNING, "anetTcpServer: %s", err);
        return 1;
    }

    anetNonBlock(NULL, listenfd);

    /* set up the listening socket which will accept connections from clients
     */
    if (aeCreateFileEvent(el, listenfd, AE_READABLE, client_listen,
                          NULL) == AE_ERR) {
        Log(WARNING, "failed to set up listen event");
        return 1;
    }

    /* set up the dispatch_connect event which will try to connect to the
     * data stream server. If it can't connect, it will retry every 10
     * seconds. */
    if (aeCreateTimeEvent(el, 0, dispatch_connect, NULL, NULL) == AE_ERR) {
        Log(WARNING, "failed to set up dispatch connect");
        return 1;
    }

    /* enter the main event loop */
    el->stop = 0;
    while (!el->stop) {
        if (el->beforesleep != NULL)
            el->beforesleep(el);
        aeProcessEvents(el, AE_ALL_EVENTS);
    }

    Log(NOTICE, "ctrl-c caught. flushing buffers...");

    time_t now = time(NULL);
    while (time(NULL) < now + 1) {
        if (aeProcessEvents(el, AE_FILE_EVENTS | AE_DONT_WAIT) == 0) break;
    }

    aeDeleteEventLoop(el);

    return 0;
}
