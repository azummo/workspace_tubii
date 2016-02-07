#include <sys/stat.h>
#include <stdio.h>
#include "anet.h"
#include "ae.h"
#include "consumer.h"
#include "record_info.h"
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "logging.h"
#include "sock.h"

extern aeEventLoop *el;

static DataConsumer *consumer_first = NULL;

int consumer_setup_listen(aeEventLoop *el, long long id, void *data)
{
    char err[ANET_ERR_LEN];

    int sock = anetTcpServer(err, CONSUMER_PORT, NULL, 10);

    if (sock == ANET_ERR) {
        Log(WARNING, "failed to set up consumer listening socket");
        /* try again in 1 second */
        return 1000;
    }

    if (anetNonBlock(err, sock) == ANET_ERR) {
        Log(WARNING, "anetNonBlock: %s", err);
        close(sock);
        return 1000;
    }

    if (aeCreateFileEvent(el, sock, AE_READABLE, consumer_accept,
                          NULL) == AE_ERR) {
        Log(WARNING, "failed to set up accept event");
        close(sock);
        return 1000;
    }

    Log(NOTICE, "set up consumer listening socket.");

    return AE_NOMORE;
}

void consumer_accept(aeEventLoop *el, int fd, void *data, int mask)
{
    char err[ANET_ERR_LEN];
    char ip[46];
    int port, sock;
    DataConsumer *c;

    if ((sock = anetTcpAccept(err, fd, ip, sizeof(ip), &port)) == ANET_ERR) {
        Log(WARNING, "anetTcpAccept: %s", err);
        return;
    }

    if ((c = consumer_init(sock)) == NULL) {
        Log(WARNING, "failed to create data consumer");
        return;
    }

    Log(NOTICE, "consumer connected from %s on port %d", ip, port);

    strcpy(c->ip, ip);
    c->port = port;
    time(&c->time_connected);

    if (aeCreateFileEvent(el, sock, AE_READABLE, consumer_read, c) == AE_ERR) {
        Log(WARNING, "failed to set up read event for consumer");
        consumer_free(c);
    }
}

DataConsumer *consumer_init(int fd)
{
    char err[ANET_ERR_LEN];

    DataConsumer *c = (DataConsumer *)malloc(sizeof(DataConsumer));
    c->sock = sock_init(fd, SEND_BUFSIZE, READ_BUFSIZE);
    c->sublen = 0;
    c->prev = NULL;
    c->next = NULL;

    if (anetNonBlock(err, fd) == ANET_ERR) {
        Log(WARNING, "failed to set consumer socket to non blocking");
        consumer_free(c);
        return NULL;
    }

    if (consumer_first == NULL) {
        consumer_first = c;
    } else {
        DataConsumer *tail = consumer_first;

        while (tail->next != NULL) tail = tail->next;

        tail->next = c;
        c->next = NULL;
        c->prev = tail;
    }

    return c;
}

void consumer_free(DataConsumer *c)
{
    aeDeleteFileEvent(el, c->sock->fd, AE_READABLE);
    aeDeleteFileEvent(el, c->sock->fd, AE_WRITABLE);

    sock_free(c->sock);

    if (c->next) c->next->prev = c->prev;
    if (c->prev) c->prev->next = c->next;

    if (consumer_first == c) consumer_first = c->next;

    free(c);
}

void consumer_read(aeEventLoop *el, int fd, void *data, int mask)
{
    struct GenericRecordHeader header;
    DataConsumer *c = (DataConsumer *)data;
    int i, sublen;
    char *record;

    if (sock_read(c->sock)) {
        Log(WARNING, "error reading from consumer: %s", sock_err);
        consumer_free(c);
        return;
    }

    while ((record = sock_read_record(c->sock, &header))) {
        /* swap to host byte order */
        header.RecordID = ntohl(header.RecordID);
        header.RecordLength = ntohl(header.RecordLength);
        header.RecordVersion = ntohl(header.RecordVersion);

        /* we have a full record */
        if (header.RecordID != 0x53436d64) {
            Log(WARNING, "unknown record type 0x%x", header.RecordID);
            consumer_reply(c, "unknown record type", 0);
        } else {
            if (header.RecordVersion != 0) {
                Log(WARNING, "unknown command type 0x%x",
                        header.RecordVersion);
                consumer_reply(c, "unknown command type", 0);
            } else {
                sublen = header.RecordLength/4;

                if (sublen > MAX_SUBLEN) {
                    Log(WARNING, "max subscription length %d", MAX_SUBLEN);
                    consumer_reply(c, "too many subscriptions", kSCmd);
                } else {
                    /* note: we keep the record ids in network byte order,
                     * so it is easy to check later */

                    c->sub_bloom = 0;
                    for (i = 0; i < header.RecordLength/4; i++) {
                        c->sub[i] = ((uint32_t *)record)[i];
                        c->sub_bloom |= c->sub[i];
                    }
                    c->sublen = sublen;
                    consumer_reply(c, "OK", kSCmd);
                }
            }
        }
    }
}

void consumer_write(aeEventLoop *el, int fd, void *data, int mask)
{
    DataConsumer *c = (DataConsumer *)data;

    if (sock_write(c->sock)) {
        Log(WARNING, "error writing to consumer: %s", sock_err);
        consumer_free(c);
        return;
    }

    if (CB_BYTES(c->sock->sendbuf) == 0) {
        aeDeleteFileEvent(el, c->sock->fd, AE_WRITABLE);
    }
}

void swap_header(struct GenericRecordHeader *header)
{
    header->RecordID = htonl(header->RecordID);
    header->RecordLength = htonl(header->RecordLength);
    header->RecordVersion = htonl(header->RecordVersion);
}

int consumer_reply(DataConsumer *c, char *msg, uint32_t cmd)
{
    struct GenericRecordHeader header = { kSRsp, strlen(msg)+1, cmd };
    swap_header(&header);
    if (consumer_send(c, (char *)&header, sizeof(header))) return -1;
    if (consumer_send(c, msg, strlen(msg)+1)) return -1;
    return 0;
}

int consumer_send(DataConsumer *c, char *buf, int len)
{
    if (sock_append(c->sock, buf, len)) {
        Log(WARNING, "failed to send to consumer: %s", sock_err);
        return -1;
    }

    if (aeCreateFileEvent(el, c->sock->fd, AE_WRITABLE, consumer_write,
                          c) == AE_ERR) {
        Log(WARNING, "error: failed to set up client write "
                        "event");
        return -1;
    }

    return 0;
}

void send_to_consumers(struct GenericRecordHeader *header, char *record)
{
    int i;
    DataConsumer *c;

    for (c = consumer_first; c != NULL; c = c->next) {
        /* use a bloom filter to quickly check if they are *not*
         * subscribed */
        if ((c->sub_bloom & header->RecordID) != header->RecordID) continue;

        for (i = 0; i < c->sublen; i++) {
            if (c->sub[i] == header->RecordID) {
                /* write record to client */
                if (sock_append_record(c->sock, header, record)) {
                    Log(WARNING, "error: failed to send record to "
                                    "consumer");
                }

                if (aeCreateFileEvent(el, c->sock->fd, AE_WRITABLE,
                                      consumer_write, c) == AE_ERR) {
                    Log(WARNING, "error: failed to set up client write event");
                }
                break;
            }
        }
    }
}
