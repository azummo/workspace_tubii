#include "ae.h"
#include <stdio.h>
#include "sock.h"
#include "anet.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "record_info.h"
#include "sds.h"
#include "logging.h"
#include "data.h"

#define BUFSIZE 4096000 /* 5 MB */

Sock *data_stream;

extern aeEventLoop *el;

int dispatch_connect(aeEventLoop *el, long long id, void *data)
{
    char err[ANET_ERR_LEN];

    /* Try to connect to the dispatcher. If it fails, try again 10 seconds
     * later */
    int sock = anetTcpConnect(err, "192.168.1.100", 4002);

    if (sock == ANET_ERR) {
        Log(WARNING, "failed to connect to data stream server: %s.", err);
        return 10000;
    }

    data_stream = sock_init(sock, BUFSIZE, 0);

    return AE_NOMORE;
}

void write_data_buffer(aeEventLoop *el, int fd, void *data, int mask)
{
    /* Write the data stream buffer to the data stream server. If it fails,
     * free the data stream connection, and try to reconnect in 10 seconds. */
    Sock *s = (Sock *)data;

    if (sock_write(s)) {
        Log(WARNING, "error sending data stream buffer");

        aeDeleteFileEvent(el, s->fd, AE_WRITABLE);

        sock_free(s);
        data_stream = NULL;
        if (aeCreateTimeEvent(el, 10000, dispatch_connect, NULL,
                              NULL) == AE_ERR) {
            Log(WARNING, "failed to create dispatch_connect event");
        }

        return;
    }

    if (CB_BYTES(s->sendbuf) == 0) {
        aeDeleteFileEvent(el, s->fd, AE_WRITABLE);
    }
}

void write_to_data_stream(struct GenericRecordHeader *header, void *record)
{
    /* Write `len` bytes from `buf` to the data stream connection buffer. */

    /* Return immediately if we aren't connected */
    if (data_stream == NULL) return;

    if (sock_append_record(data_stream, header, record)) {
        Log(WARNING, "write to data stream: %s", sock_err);
    }

    if (aeCreateFileEvent(el, data_stream->fd, AE_WRITABLE, write_data_buffer,
                          data_stream) != AE_OK) {
        Log(WARNING, "failed to create write event for data stream "
                        "connection");
    }
}

