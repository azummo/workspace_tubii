#ifndef PRODUCER_H
#define PRODUCER_H

#include "ae.h"
#include <stdint.h>
#include "sock.h"

#define PRODUCER_PORT 4002

typedef struct DataProducer {
    Sock *sock;
    char ip[46];
    int port;
    time_t time_connected;
} DataProducer;

/* set up the listening socket for incoming producer connections */
int producer_setup_listen(aeEventLoop *el, long long id, void *data);
/* accept a new producer connection */
void producer_accept(aeEventLoop *el, int fd, void *data, int mask);

DataProducer *producer_init(int fd);
void producer_free(DataProducer *c);
void producer_read(aeEventLoop *el, int fd, void *data, int mask);

#endif
