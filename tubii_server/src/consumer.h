#ifndef CONSUMER_H
#define CONSUMER_H

#include "ae.h"
#include <stdint.h>
#include "record_info.h"
#include "sock.h"

#define MAX_SUBLEN 100
#define READ_BUFSIZE 4096
#define SEND_BUFSIZE 409600
#define READLEN 256

#define CONSUMER_PORT 4000

typedef struct DataConsumer {
    Sock *sock;
    char ip[46];
    int port;
    time_t time_connected;

    uint32_t sub[MAX_SUBLEN];
    int sublen;
    uint32_t sub_bloom;
    struct DataConsumer *prev;
    struct DataConsumer *next;
} DataConsumer;

void swap_header(struct GenericRecordHeader *header);

/* set up the listening socket for incoming consumer connections */
int consumer_setup_listen(aeEventLoop *el, long long id, void *data);
/* accept a new consumer connection */
void consumer_accept(aeEventLoop *el, int fd, void *data, int mask);

DataConsumer *consumer_init(int fd);
void consumer_free(DataConsumer *c);

void consumer_read(aeEventLoop *el, int fd, void *data, int mask);
void consumer_write(aeEventLoop *el, int fd, void *data, int mask);
int consumer_reply(DataConsumer *c, char *msg, uint32_t cmd);
int consumer_send(DataConsumer *c, char *buf, int len);

void send_to_consumers(struct GenericRecordHeader *header, char *record);

#endif
