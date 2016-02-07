#ifndef TUBII_CLIENT_H
#define TUBII_CLIENT_H

#include "sds.h"
#include "ae.h"
#include <stdint.h>

typedef struct {
    int fd;
    sds querybuf;
    char *buf;
    int bufpos;
    int buflen;
} Client;

typedef void command_func(Client *c, sds *argv);

typedef struct Command {
    char *name;
    command_func *func;
    int arity;
} Command;

extern aeEventLoop *el;

#define READ_SIZE 256
#define MAX_QUERY_SIZE 256

int safe_strtoul(char *s, uint32_t *si);
int safe_strtoull(char *s, uint64_t *si);
int safe_strtof(char *s, float *f);

void help(Client *c, sds *argv);
void ping(Client *c, sds *argv);
void MZHappy(Client *c, sds *argv);
void smelliepulser(Client *c, sds *argv);
void smelliedelay(Client *c, sds *argv);
void telliepulser(Client *c, sds *argv);
void telliedelay(Client *c, sds *argv);
void genericpulser(Client *c, sds *argv);
void genericdelay(Client *c, sds *argv);
void gtdelay(Client *c, sds *argv);
void initialise(Client *c, sds *argv);
void loadShift(Client *c, sds *argv);
void readShift(Client *c, sds *argv);
void muxer(Client *c, sds *argv);
void muxenable(Client *c, sds *argv);
void trigBurst(Client *c, sds *argv);
void trigCombo(Client *c, sds *argv);
void trigPrescale(Client *c, sds *argv);
void tubiiClock(Client *c, sds *argv);
void displayCount(Client *c, sds *argv);
void trigMask(Client *c, sds *argv);
void trigPoll(Client *c, sds *argv);
void gtpoll(Client *c, sds *argv);
void dataready(Client *c, sds *argv);
void clockreset(Client *c, sds *argv);
void clockstatus(Client *c, sds *argv);
void test(Client *c, sds *argv);
void yup(Client *c, sds *argv);

Client *init_client(int fd, int buflen);
void free_client(Client *c);
void add_reply(Client *c, char *fmt, ...);
void read_client(aeEventLoop *el, int fd, void *data, int mask);
int send_buffer(Client *c);
void write_client(aeEventLoop *el, int fd, void *data, int mask);
int process_buffer(Client *c);
void process_command(Client *c, int argc, sds *argv);

#endif
