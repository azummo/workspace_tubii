/* blocked.c - generic support for blocking operations like BLPOP & WAIT.
 *
 * Copyright (c) 2009-2012, Salvatore Sanfilippo <antirez at gmail dot com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * ---------------------------------------------------------------------------
 *
 * API:
 *
 * getTimeoutFromObjectOrReply() is just an utility function to parse a
 * timeout argument since blocking operations usually require a timeout.
 *
 * blockClient() set the CLIENT_BLOCKED flag in the client, and set the
 * specified block type 'btype' filed to one of BLOCKED_* macros.
 *
 * unblockClient() unblocks the client doing the following:
 * 1) It calls the btype-specific function to cleanup the state.
 * 2) It unblocks the client by unsetting the CLIENT_BLOCKED flag.
 * 3) It puts the client into a list of just unblocked clients that are
 *    processed ASAP in the beforeSleep() event loop callback, so that
 *    if there is some query buffer to process, we do it. This is also
 *    required because otherwise there is no 'readable' event fired, we
 *    already read the pending commands. We also set the CLIENT_UNBLOCKED
 *    flag to remember the client is in the unblocked_clients list.
 *
 * processUnblockedClients() is called inside the beforeSleep() function
 * to process the query buffer from unblocked clients and remove the clients
 * from the blocked_clients queue.
 *
 * replyToBlockedClientTimedOut() is called by the cron function when
 * a client blocked reaches the specified timeout (if the timeout is set
 * to 0, no timeout is processed).
 * It usually just needs to send a reply to the client.
 *
 * When implementing a new type of blocking opeation, the implementation
 * should modify unblockClient() and replyToBlockedClientTimedOut() in order
 * to handle the btype-specific behavior of this two functions.
 * If the blocking operation waits for certain keys to change state, the
 * clusterRedirectBlockedClientIfNeeded() function should also be updated.
 */

#include "server.h"

/* Block a client for an operation which needs to run over several loops
 * of the event loop. If bfree is not NULL, that function will be called
 * if the client disconnects while it is still blocked. This is useful
 * to clean up any pointers to the client before it is freed. */
void blockClient(client *c, blockingFreeProc *bfree, void *data) {
    c->flags |= CLIENT_BLOCKED;

    if (bfree) {
        c->bfree = bfree;
        c->bpop.data = data;
    } else {
        c->bfree = NULL;
        c->bpop.data = NULL;
    }

    server.bpop_blocked_clients++;
}

/* This function is called in the beforeSleep() function of the event loop
 * in order to process the pending input buffer of clients that were
 * unblocked after a blocking operation. */
void processUnblockedClients(void) {
    listNode *ln;
    client *c;

    while (listLength(server.unblocked_clients)) {
        ln = listFirst(server.unblocked_clients);
        serverAssert(ln != NULL);
        c = ln->value;
        listDelNode(server.unblocked_clients,ln);
        c->flags &= ~CLIENT_UNBLOCKED;

        /* Process remaining data in the input buffer, unless the client
         * is blocked again. Actually processInputBuffer() checks that the
         * client is not blocked before to proceed, but things may change and
         * the code is conceptually more correct this way. */
        if (!(c->flags & CLIENT_BLOCKED)) {
            if (c->querybuf && sdslen(c->querybuf) > 0) {
                processInputBuffer(c);
            }
        }
    }
}

/* Unblock a client calling the right function depending on the kind
 * of operation the client is blocking for. */
void unblockClient(client *c) {
    /* Clear the flags, and put the client in the unblocked list so that
     * we'll process new commands in its query buffer ASAP. */
    c->flags &= ~CLIENT_BLOCKED;

    c->bfree = NULL;
    c->bpop.data = NULL;

    server.bpop_blocked_clients--;
    /* The client may already be into the unblocked list because of a previous
     * blocking operation, don't add back it into the list multiple times. */
    if (!(c->flags & CLIENT_UNBLOCKED)) {
        c->flags |= CLIENT_UNBLOCKED;
        listAddNodeTail(server.unblocked_clients,c);
    }
}

/* Mass-unblock clients because something changed in the instance that makes
 * blocking no longer safe. For example clients blocked in list operations
 * in an instance which turns from master to slave is unsafe, so this function
 * is called when a master turns into a slave.
 *
 * The semantics is to send an -UNBLOCKED error to the client, disconnecting
 * it at the same time. */
void disconnectAllBlockedClients(void) {
    listNode *ln;
    listIter li;

    listRewind(server.clients,&li);
    while((ln = listNext(&li))) {
        client *c = listNodeValue(ln);

        if (c->flags & CLIENT_BLOCKED) {
            addReplySds(c,sdsnew(
                "-UNBLOCKED force unblock from blocking operation, "
                "instance state changed (master -> slave?)\r\n"));
            unblockClient(c);
            c->flags |= CLIENT_CLOSE_AFTER_REPLY;
        }
    }
}
