#ifndef SOCK_H
#define SOCK_H

#include "ae.h"
#include "record_info.h"

#define MOD(a,b) (((a % b) + b) % b)

#define CB_BYTES(cb) MOD(cb->tail - cb->head, cb->size)
#define CB_BYTES_TO_END(cb) ((cb->tail >= cb->head) ? \
                                (cb->tail - cb->head) : (cb->size - cb->head))
/* we reserve one byte to distinguish empty/full */
#define CB_SPACE(cb) (cb->size - CB_BYTES(cb) - 1)
#define CB_SPACE_TO_END(cb) ((cb->tail >= cb->head) ? \
                                ((cb->head == 0) ? (cb->size - cb->tail - 1) : \
                                    (cb->size - cb->tail)) : \
                                (cb->head - cb->tail))

typedef struct CircularBuffer {
    char *buf;
    int head;
    int tail;
    int size;
} CircularBuffer;

typedef struct Sock {
    int fd;
    int missed_packets;
    CircularBuffer *sendbuf;
    CircularBuffer *recvbuf;
} Sock;

char sock_err[256];

CircularBuffer *cb_init(int size);
void cb_free(CircularBuffer *cb);

Sock *sock_init(int fd, int sendbufsize, int recvbufsize);
void sock_free(Sock *s);
int sock_append(Sock *s, void *msg, int len);
int sock_write(Sock *s);
int sock_read(Sock *s);
char *sock_read_record(Sock *s, struct GenericRecordHeader *header);
int sock_append_record(Sock *s, struct GenericRecordHeader *header,
                       void *record);

#endif
