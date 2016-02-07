#include <sys/stat.h>
#include <stdio.h>
#include "anet.h"
#include "ae.h"
#include "consumer.h"
#include "producer.h"
#include "record_info.h"
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "logging.h"
#include "sock.h"

#define RECV_BUFSIZE 40960
#define READLEN 256

extern aeEventLoop *el;

int producer_setup_listen(aeEventLoop *el, long long id, void *data)
{
    char err[ANET_ERR_LEN];

    int sock = anetTcpServer(err, PRODUCER_PORT, NULL, 10);

    if (sock == ANET_ERR) {
        Log(WARNING, "failed to set up producer listening socket");
        /* try again in 1 second */
        return 1000;
    }

    if (anetNonBlock(err, sock) == ANET_ERR) {
        Log(WARNING, "anetNonBlock: %s", err);
        close(sock);
        return 1000;
    }

    if (aeCreateFileEvent(el, sock, AE_READABLE, producer_accept,
                          NULL) == AE_ERR) {
        Log(WARNING, "failed to set up accept event");
        close(sock);
        return 1000;
    }

    Log(NOTICE, "set up producer listening socket");

    return AE_NOMORE;
}

void producer_accept(aeEventLoop *el, int fd, void *data, int mask)
{
    char err[ANET_ERR_LEN];
    char ip[46];
    int port, sock;
    DataProducer *c;

    if ((sock = anetTcpAccept(err, fd, ip, sizeof(ip), &port)) == ANET_ERR) {
        Log(WARNING, "anetTcpAccept: %s", err);
        return;
    }

    if ((c = producer_init(sock)) == NULL) {
        Log(WARNING, "failed to set up data producer");
        return;
    }

    Log(NOTICE, "producer connected from %s on port %d", ip, port);

    strcpy(c->ip, ip);
    c->port = port;
    time(&c->time_connected);

    if (aeCreateFileEvent(el, sock, AE_READABLE, producer_read, c)) {
        Log(WARNING, "failed to set up read event for producer");
        producer_free(c);
    }
}

DataProducer *producer_init(int fd)
{
    char err[ANET_ERR_LEN];

    DataProducer *c = (DataProducer *)malloc(sizeof(DataProducer));
    c->sock = sock_init(fd, 0, RECV_BUFSIZE);

    if (anetNonBlock(err, fd) == ANET_ERR) {
        Log(WARNING, "failed to set producer socket to non blocking");
        producer_free(c);
        return NULL;
    }

    return c;
}

void producer_free(DataProducer *c)
{
    aeDeleteFileEvent(el, c->sock->fd, AE_READABLE);
    aeDeleteFileEvent(el, c->sock->fd, AE_WRITABLE);

    sock_free(c->sock);

    free(c);
}

void producer_read(aeEventLoop *el, int fd, void *data, int mask)
{
    struct GenericRecordHeader header;
    DataProducer *c = (DataProducer *)data;
    uint32_t record_length;
    char *record;

    if (sock_read(c->sock)) {
        Log(WARNING, "error reading from producer: %s", sock_err);
        producer_free(c);
        return;
    }

    while ((record = sock_read_record(c->sock, &header))) {
        record_length = ntohl(header.RecordLength);

        send_to_consumers(&header, record);
    }
}

