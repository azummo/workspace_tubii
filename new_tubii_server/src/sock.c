#include "sock.h"
#include <stdlib.h>
#include <unistd.h>
#include "logging.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "anet.h"
#include "record_info.h"
#include <arpa/inet.h>

CircularBuffer *cb_init(int size)
{
    CircularBuffer *cb = (CircularBuffer *)malloc(sizeof(CircularBuffer));
    cb->buf = malloc(size);
    cb->size = size;
    cb->head = 0;
    cb->tail = 0;

    return cb;
}

void cb_free(CircularBuffer *cb)
{
    free(cb->buf);
    free(cb);
}

Sock *sock_init(int fd, int sendbufsize, int recvbufsize)
{
    Sock *s = (Sock *)malloc(sizeof(Sock));
    s->fd = fd;

    s->sendbuf = cb_init(sendbufsize);
    s->recvbuf = cb_init(recvbufsize);

    s->missed_packets = 0;

    anetNonBlock(NULL, fd);

    return s;
}

void sock_free(Sock *s)
{
    close(s->fd);
    cb_free(s->sendbuf);
    cb_free(s->recvbuf);
    free(s);
}

int sock_append(Sock *s, void *msg, int len)
{
    if (len > CB_SPACE(s->sendbuf)) {
        sprintf(sock_err, "not enough room in send buffer");
        return -1;
    }

    if (CB_SPACE_TO_END(s->sendbuf) > len) {
        memcpy(s->sendbuf->buf + s->sendbuf->tail, msg, len);
    } else {
        int a = CB_SPACE_TO_END(s->sendbuf);
        int b = len - a;
        memcpy(s->sendbuf->buf+s->sendbuf->tail, msg, a);
        memcpy(s->sendbuf->buf, msg+a, b);
    }

    s->sendbuf->tail = MOD(s->sendbuf->tail + len, s->sendbuf->size);

    return 0;
}

int sock_append_record(Sock *s, struct GenericRecordHeader *header,
                       void *record)
{
    uint32_t len = ntohl(header->RecordLength);

    if (sizeof(struct GenericRecordHeader) + len > CB_SPACE(s->sendbuf)) {
        sprintf(sock_err, "buffer is full");
        s->missed_packets += 1;
        return -1;
    }

    if (sock_append(s, header, sizeof(struct GenericRecordHeader))) {
        sprintf(sock_err, "failed to append header");
        return -1;
    }

    if (sock_append(s, record, len)) {
        sprintf(sock_err, "failed to append record");
        return -1;
    }

    return 0;
}

char *sock_read_record(Sock *s, struct GenericRecordHeader *header)
{
    /* If there is a full record in the buffer, copy the header and return
     * a pointer to the start of the record, otherwise return NULL.
     * Note: the returned pointer may be overwritten on subsequent loops, so
     * if you need the record to persist, you must memcpy it!
     */
    char *record;
    int record_length, bytes = CB_BYTES_TO_END(s->recvbuf);

    if (bytes >= sizeof(struct GenericRecordHeader)) {
        *header = *((struct GenericRecordHeader *)(s->recvbuf->buf + s->recvbuf->head));
    } else {
        return NULL;
    }

    record_length = ntohl(header->RecordLength);

    if (bytes < sizeof(struct GenericRecordHeader) + record_length) return NULL;

    record = s->recvbuf->buf + s->recvbuf->head + sizeof(struct GenericRecordHeader);
    s->recvbuf->head += sizeof(struct GenericRecordHeader) + record_length;
    return record;
}

int sock_read(Sock *s)
{
    int bytes, nread;

    if (s->recvbuf->head != 0) {
        /* make sure the head pointer is at the start */
        memmove(s->recvbuf->buf, s->recvbuf->buf + s->recvbuf->head, CB_BYTES(s->recvbuf));
        s->recvbuf->tail -= s->recvbuf->head;
        s->recvbuf->head = 0;
    }

    while (1) {
        if ((bytes = CB_SPACE_TO_END(s->recvbuf)) == 0) break;

        nread = read(s->fd, s->recvbuf->buf + s->recvbuf->tail, bytes);

        if (nread == -1) {
            if (errno == EAGAIN) {
                nread = 0;
                break;
            } else {
                sprintf(sock_err, "sock_read: %s", strerror(errno));
                return -1;
            }
        } else if (nread == 0) {
            sprintf(sock_err, "disconnect");
            return -1;
        }

        s->recvbuf->tail += nread;
    }

    return 0;
}
    
int sock_write(Sock *s)
{
    int bytes, sent;

    while (1) {
        if ((bytes = CB_BYTES_TO_END(s->sendbuf)) == 0) break;

        sent = write(s->fd, s->sendbuf->buf+s->sendbuf->head, bytes);

        if (sent == -1) {
            if (errno == EAGAIN) {
                sent = 0;
                break;
            } else {
                Log(WARNING, "write: %s", strerror(errno));
                return -1;
            }
        }

        s->sendbuf->head += sent;

        if (s->sendbuf->head == s->sendbuf->size) s->sendbuf->head = 0;
    }

    if (s->sendbuf->head == s->sendbuf->tail) s->sendbuf->head = s->sendbuf->tail = 0;

    return 0;
}
