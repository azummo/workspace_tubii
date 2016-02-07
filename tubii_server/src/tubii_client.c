#include "ae.h"
#include <string.h>
#include "sds.h"
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <stdint.h>
#include "tubii_client.h"
#include <stdio.h>
#include <string.h>
#include "anet.h"
#include <time.h>
#include <arpa/inet.h>
#include "data.h"
#include "logging.h"
#include "tubiiAddresses.h"
#include "tubiiClocks.h"
#include "tubiiELLIE.h"
#include "tubiiRegs.h"
#include "tubiiTriggers.h"
#include "tubiiUtil.h"

Command command_table[] = {
	{"ping",         ping,         0},
	{"MZHappy",      MZHappy,      0},
    {"initialise",	 initialise,     0},
	{"smelliepulser",smelliepulser,2},
	{"smelliedelay", smelliedelay, 1},
	{"telliepulser", telliepulser, 2},
	{"telliedelay",  telliedelay,  1},
	{"genericpulser",   genericpulser,   2},
	{"genericdelay",    genericdelay,    1},
	{"loadShift",  	 loadShift,    1},
	{"readShift",  	 readShift,    0},
	{"muxer",   	 muxer,        1},
	{"muxenable",    muxenable,    1},
	{"trigBurst",    trigBurst,    1},
	{"trigCombo",  	 trigCombo,    1},
	{"trigPrescale", trigPrescale, 1},
	{"tubiiClock",   tubiiClock,   1},
	{"displayCount", displayCount, 1},
	{"trigMask",	 trigMask,     1},
	{"trigPoll",	 trigPoll,     0},
	{"gtpoll",       gtpoll,       2},
	{"gtdelay",      gtdelay,      1},
	{"dataready",    dataready,    1},
	{"clockReset",   clockreset,   1},
	{"clockStatus",  clockstatus,  0},
	{"help",  help, 0},
	{"test", test, 0},
	{"yup", yup, 0}
};

void test(Client *c, sds *argv)
{
	Testing(GTPOLL_BASEADDR,GTPOLL_HIGHADDR);
	printf("GTPoll\n");
	add_reply(c, "+OK GTPoll");
	sleep(1);

	Testing(BURSTTRIG_BASEADDR,BURSTTRIG_HIGHADDR);
	printf("Burst\n");
	add_reply(c, "+OK TriggerBurst");
	sleep(1);

	Testing(CLOCKLOGIC_BASEADDR,CLOCKLOGIC_HIGHADDR);
	printf("Clock\n");
	add_reply(c, "+OK ClockLogic");
	sleep(1);

	Testing(COMBOTRIG_BASEADDR,COMBOTRIG_HIGHADDR);
	printf("Combo\n");
	add_reply(c, "+OK TriggerCombo");
	sleep(1);

	Testing(COUNTDISP_BASEADDR,COUNTDISP_HIGHADDR);
	printf("Count\n");
	add_reply(c, "+OK CountDisp");
	sleep(1);

	Testing(PRESCALETRIG_BASEADDR,PRESCALETRIG_HIGHADDR);
	printf("Prescale\n");
	add_reply(c, "+OK TriggerPrescale");
	sleep(1);

	Testing(READSHIFT_BASEADDR,READSHIFT_HIGHADDR);
	printf("Read\n");
	add_reply(c, "+OK ReadShift");
	sleep(1);

	Testing(GENERICDELAY_BASEADDR,GENERICDELAY_HIGHADDR);
	printf("GDelay\n");
	add_reply(c, "+OK GenericDelay");
	sleep(1);

	Testing(GTDELAY_BASEADDR,GTDELAY_HIGHADDR);
	printf("GTDelay\n");
	add_reply(c, "+OK GTDelay");
	sleep(1);

	Testing(SMELLIEDELAY_BASEADDR,SMELLIEDELAY_HIGHADDR);
	printf("SMDelay\n");
	add_reply(c, "+OK SmellieDelay");
	sleep(1);

	Testing(TELLIEDELAY_BASEADDR,TELLIEDELAY_HIGHADDR);
	printf("TDelay\n");
	add_reply(c, "+OK TellieDelay");
	sleep(1);

	Testing(FIFO_BASEADDR,FIFO_HIGHADDR);
	printf("Fifo\n");
	add_reply(c, "+OK Fifo");
	sleep(1);

	Testing(SHIFTREGS_BASEADDR,SHIFTREGS_HIGHADDR);
	printf("Shifty\n");
	add_reply(c, "+OK Shifty");
	sleep(1);

	Testing(MZHAPPY_BASEADDR,MZHAPPY_HIGHADDR);
	printf("MZHappy\n");
	add_reply(c, "+OK MZHappy");
	sleep(1);

	Testing(GENERICPULSER_BASEADDR,GENERICPULSER_HIGHADDR);
	printf("GPulser\n");
	add_reply(c, "+OK GenericPulser");
	sleep(1);

	Testing(SMELLIEPULSER_BASEADDR,SMELLIEPULSER_HIGHADDR);
	printf("SMPulser\n");
	add_reply(c, "+OK SmelliePulser");
	sleep(1);

	Testing(TELLIEPULSER_BASEADDR,TELLIEPULSER_HIGHADDR);
	printf("TPulser\n");
	add_reply(c, "+OK TelliePulser");
	sleep(1);

	Testing(TRIGGERSPLIT_BASEADDR,TRIGGERSPLIT_HIGHADDR);
	printf("TSplit\n");
	add_reply(c, "+OK TriggerSplit");
	sleep(1);

	Testing(TRIGGEROUT_BASEADDR,TRIGGEROUT_HIGHADDR);
	printf("TOut\n");
	add_reply(c, "+OK TriggerOut");
}

void help(Client *c, sds *argv)
{
	add_reply(c, "Try something else.");
    add_reply(c, "+PING");
}

void yup(Client *c, sds *argv)
{
    add_reply(c, "+YUP");
}

void ping(Client *c, sds *argv)
{
    add_reply(c, "+PING");
}

void MZHappy(Client *c, sds *argv)
{
	int ret= Pulser("0","0",MZHAPPY_BASEADDR,MZHAPPY_HIGHADDR);

	if(ret == 0) add_reply(c, "+OK");
	else add_reply(c, "-ERR MZ Unhappy");
}

void initialise(Client *c, sds *argv)
{
	//Put caen in attenuating mode
	Muxer("1");
	MuxEnable("1");
	LoadShift("255");
	LoadShift("255");
	MuxEnable("0");
	DataReady("6");
	DataReady("4");
	//setup DGT and LO* delay lengths
	Muxer("3");
	MuxEnable("1");
	LoadShift("153");
	LoadShift("153");
	MuxEnable("0");
	//Set MTCA MIMIC DAC value
	Muxer("2");
	MuxEnable("1");
	MuxEnable("0");
	LoadShift("153");
	LoadShift("153");
	DataReady("0");
	DataReady("4");

	int ret= 0;
	if(ret == 0) add_reply(c, "+OK");
	else add_reply(c, "-ERR MZ Unhappy");
}

void smelliepulser(Client *c, sds *argv)
{
	int ret= Pulser(argv[0],argv[1],SMELLIEPULSER_BASEADDR,SMELLIEPULSER_HIGHADDR);

	if(ret == 0) add_reply(c, "+OK");
	else add_reply(c, "-ERR Pulser rate is outside valid range");
}

void smelliedelay(Client *c, sds *argv)
{
	int ret= Delay(argv[0],SMELLIEDELAY_BASEADDR,SMELLIEDELAY_HIGHADDR);

	if(ret == 0) add_reply(c, "+OK");
	else add_reply(c, "-ERR Delay length is outside valid range");
}

void telliepulser(Client *c, sds *argv)
{
	int ret= Pulser(argv[0],argv[1],TELLIEPULSER_BASEADDR,TELLIEPULSER_HIGHADDR);

	if(ret == 0) add_reply(c, "+OK");
	else add_reply(c, "-ERR Pulser rate is outside valid range");
}

void telliedelay(Client *c, sds *argv)
{
	int ret= Delay(argv[0],TELLIEDELAY_BASEADDR,TELLIEDELAY_HIGHADDR);

	if(ret == 0) add_reply(c, "+OK");
	else add_reply(c, "-ERR Delay length is outside valid range");
}

void genericpulser(Client *c, sds *argv)
{
	// Need to split argument into the sync part and async part
	// Sync part:
	int ret1= Pulser(argv[0],argv[1],GENERICPULSER_BASEADDR,GENERICPULSER_HIGHADDR);

	// Send async part to shift reg:
	int ret2= Muxer("4");
	int ret3= LoadShift(argv[0]);

	int ret=0;
	if(ret1 || ret2 || ret3) ret=-1;

	if(ret == 0) add_reply(c, "+OK");
	else add_reply(c, "-ERR Pulser rate is outside valid range");
}

void genericdelay(Client *c, sds *argv)
{
	// Need to split argument into the sync part and async part
	// Sync part:
	int ret1= Delay(argv[0],GENERICDELAY_BASEADDR,GENERICDELAY_HIGHADDR);

	// Send async part to shift reg:
	int ret2= Muxer("5");
	int ret3= LoadShift(argv[0]);

	int ret=0;
	if(ret1 || ret2 || ret3) ret=-1;

	if(ret == 0) add_reply(c, "+OK");
	else add_reply(c, "-ERR Delay length is outside valid range");
}

void gtdelay(Client *c, sds *argv)
{
	int ret= Delay(argv[0],GTDELAY_BASEADDR,GTDELAY_HIGHADDR);

	if(ret == 0) add_reply(c, "+OK");
	else add_reply(c, "-ERR Delay length is outside valid range");
}

void loadShift(Client *c, sds *argv)
{
	int ret= LoadShift(argv[0]);

	if(ret == 0) add_reply(c, "+OK");
	else add_reply(c, "-ERR Shift reg data out of range.");
}

void readShift(Client *c, sds *argv)
{
	int ret= ReadShift();

	if(ret != -1) add_reply(c, "+OK");
	else add_reply(c, "-ERR Shift reg data out of range.");
}

void muxer(Client *c, sds *argv)
{
	int ret= Muxer(argv[0]);

	if(ret == 0) add_reply(c, "+OK");
	else add_reply(c, "-ERR Muxer can only be set to a value between 0 and 7.");
}

void muxenable(Client *c, sds *argv)
{
	int ret= MuxEnable(argv[0]);

	if(ret == 0) add_reply(c, "+OK");
	else add_reply(c, "-ERR Muxenable can only be set to 0 and 1.");
}

void trigBurst(Client *c, sds *argv)
{
	burstTrig(argv[0]);
	add_reply(c, "+OK");
}

void trigCombo(Client *c, sds *argv)
{
	comboTrig(argv[0]);
	add_reply(c, "+OK");
}

void trigPrescale(Client *c, sds *argv)
{
	prescaleTrig(argv[0]);
	add_reply(c, "+OK");
}

void gtpoll(Client *c, sds *argv)
{
	GTpoll(argv[0],argv[1]);
	add_reply(c, "+OK");
}

void dataready(Client *c, sds *argv)
{
	int ret= DataReady(argv[0]);

	if(ret==-1) add_reply(c, "-ERR Invalid argument to dataready");
	else add_reply(c, "+OK");
}

void clockreset(Client *c, sds *argv)
{
	int ret= clockReset(argv[0]);

	if(ret != 0){
	  add_reply(c, "-ERR Invalid clock choice %i. Choose 0 or 1.", ret);
	  return;
	}

	add_reply(c, "+OK");
}

void clockstatus(Client *c, sds *argv)
{
	int ret= clockStatus();

	// do something with ret which is the status of the backup clock
	// add to the datastream?

	add_reply(c, "+OK");
}

void tubiiClock(Client *c, sds *argv)
{
	u32 BaseAddress= CLOCKLOGIC_BASEADDR;
	u32 HighAddress= CLOCKLOGIC_HIGHADDR;
	void *MappedBaseAddress= MemoryMapping(BaseAddress,HighAddress);

	int clk_choice= atoi(argv[0]);
	if(clk_choice==1 || clk_choice==0) printf("Clock choice = %i\n", clk_choice);
	else{
	  add_reply(c, "-ERR Invalid clock choice %i. Choose 0 or 1.", clk_choice);
	  return;
	}

	/// Write to the register 0, indicates which clock we want to use
	mWriteReg(MappedBaseAddress, RegOffset0, clk_choice);
	printf("Choose clock: %d\n",  mReadReg(MappedBaseAddress, clk_choice));

	munmap((void*)MappedBaseAddress, (HighAddress-BaseAddress));
	add_reply(c, "+OK");
}

void displayCount(Client *c, sds *argv)
{
	u32 BaseAddress= COUNTDISP_BASEADDR;
	u32 HighAddress= COUNTDISP_HIGHADDR;
	void *MappedBaseAddress= MemoryMapping(BaseAddress,HighAddress);
	InitialiseRegs(MappedBaseAddress);

	u32 length= (u32) atoi(argv[0]);
	mWriteReg(MappedBaseAddress, RegOffset3, length);

	while(1){
	  printf("%x \t %x\n", mReadReg(MappedBaseAddress, RegOffset0), mReadReg(MappedBaseAddress, RegOffset3));
	}

	munmap((void*)MappedBaseAddress, (HighAddress-BaseAddress));
	add_reply(c, "+OK");
}

void trigMask(Client *c, sds *argv)
{
	int ret= triggerMask(argv[0]);

	if(ret == 0) add_reply(c, "+OK");
	else add_reply(c, "+ERR, Should never fail.");
}

void trigPoll(Client *c, sds *argv)
{
		/* Send a TUBii_RECORD to the data stream server */
    struct GenericRecordHeader header;
    struct TubiiRecord trec;

    /* fill struct */
	int ret= triggerReport();

    trec.GTID = (ret & 0xFF000000) >> 24;
    trec.TrigWord = ret & 0xFFFFFF;

    header.RecordID = htonl(TUBII_RECORD);
    header.RecordLength = htonl(sizeof(trec));
    header.RecordVersion = htonl(RECORD_VERSION);

    /* convert to big endian */
    int i;
    for (i = 0; i < sizeof(trec)/4; i++) {
        ((uint32_t *)&trec)[i] = htonl(((uint32_t *)&trec)[i]);
    }

    write_to_data_stream(&header, &trec);

	add_reply(c, "+OK");
}





Client *init_client(int fd, int buflen)
{
    Client *c = malloc(sizeof(Client));
    c->buf = malloc(buflen);
    c->buflen = buflen;
    c->bufpos = 0;
    c->fd = fd;

    anetNonBlock(NULL, fd);

    if (aeCreateFileEvent(el, fd, AE_READABLE, read_client, c) == AE_ERR) {
        close(fd);
        free_client(c);
        return NULL;
    }

    c->querybuf = sdsempty();

    return c;
}

void free_client(Client *c)
{
    free(c->buf);
    aeDeleteFileEvent(el, c->fd, AE_READABLE);
    aeDeleteFileEvent(el, c->fd, AE_WRITABLE);
    sdsfree(c->querybuf);
    free(c);
}

int safe_strtoull(char *s, uint64_t *si)
{
    /* Convert string s -> 64 bit unsigned integer. */
    char *end;
    
    errno = 0;
    *si = strtoull(s, &end, 0);

    if (end == s) {
        return -1;
    } else if ('\0' != *end) {
        return -1;
    } else if (errno == ERANGE) {
        return -1;
    }

    return 0;
}

void add_reply(Client *c, char *fmt, ...)
{
    size_t l, j;
    va_list ap;
    va_start(ap, fmt);
    sds s = sdscatvprintf(sdsempty(), fmt, ap);
    va_end(ap);
    l = sdslen(s);
    for (j = 0; j < l; j++) {
        if (s[j] == '\r' || s[j] == '\n') s[j] = ' ';
    }

    s = sdscat(s, "\r\n");

    if ((c->bufpos + sdslen(s)) > c->buflen) {
        Log(WARNING, "client write buffer too small");
        return;
    }

    memcpy(c->buf+c->bufpos, s, sdslen(s));
    c->bufpos += sdslen(s);
    sdsfree(s);
}

void read_client(aeEventLoop *el, int fd, void *data, int mask)
{
    /* Read data from client socket and process any requests. */
    int nread, readlen = READ_SIZE;
    size_t qblen;

    Client *c = (Client *)data;

    qblen = sdslen(c->querybuf);
    c->querybuf = sdsMakeRoomFor(c->querybuf, readlen);
    nread = read(fd, c->querybuf+qblen, readlen);

    if (nread == -1) {
        if (errno != EAGAIN) {
            Log(WARNING, "error reading from client: %s", strerror(errno));
            free_client(c);
            return;
        }
    } else if (nread == 0) {
        Log(NOTICE, "client disconnected");
        free_client(c);
        return;
    }

    sdsIncrLen(c->querybuf, nread);

    while (sdslen(c->querybuf)) {
        if (process_buffer(c) == -1) break;
    }

    if (c->bufpos > 0) {
        if (aeCreateFileEvent(el, fd, AE_WRITABLE, write_client, c) != AE_OK) {
            Log(WARNING, "failed to create write event for client");
        }
    }
}

void write_client(aeEventLoop *el, int fd, void *data, int mask)
{
    Client *c = (Client *)data;

    if (send_buffer(c)) {
        Log(WARNING, "error writing to client");
        free_client(c);
    }
}

int send_buffer(Client *c)
{
    /* Send bytes from the client buffer. On error, return -1, else return 0. */
    int nwritten, sent = 0;

    while (sent < c->bufpos) {
        nwritten = write(c->fd, c->buf+sent, c->bufpos - sent);
        if (nwritten == -1) {
            if (errno == EAGAIN) {
                nwritten = 0;
                break;
            } else {
                Log(WARNING, "error writing to client: %s", strerror(errno));
                return -1;
            }
        }

        sent += nwritten;
    }

    if (sent == c->bufpos) {
        /* sent all the bytes */
        c->bufpos = 0;
        aeDeleteFileEvent(el, c->fd, AE_WRITABLE);
    } else {
        /* sent some of the bytes */
        memmove(c->buf, c->buf+sent, c->bufpos - sent);
        c->bufpos -= sent;
    }

    return 0;
}

int process_buffer(Client *c)
{
    /* Process the client's input buffer. Returns -1 when there is an error
     * or no more commands. */

    char *newline;
    int argc;
    sds *argv, aux;
    size_t querylen;

    newline = strchr(c->querybuf, '\n');

    if (newline == NULL) {
        if (sdslen(c->querybuf) > MAX_QUERY_SIZE) {
            Log(WARNING, "client query buffer too big");
            close(c->fd);
            free_client(c);
        }
        return -1;
    }

    if (newline && newline != c->querybuf && *(newline-1) == '\r')
        newline--;

    /* split the input buffer up to \r\n */
    querylen = newline-(c->querybuf);
    aux = sdsnewlen(c->querybuf, querylen);
    argv = sdssplitargs(aux, &argc);
    sdsfree(aux);

    /* leave data after the first line of the query in the buffer */
    sdsrange(c->querybuf, querylen+2, -1);

    if (argv == NULL) {
        add_reply(c, "-ERR unbalanced quotes in request");
        return 0;
    }

    process_command(c, argc, argv);

    sdsfreesplitres(argv, argc);

    return 0;
}

void process_command(Client *c, int argc, sds *argv)
{
    int i, ncommands = sizeof(command_table)/sizeof(Command);

    for (i = 0; i < ncommands; i++) {
        Command cmd = command_table[i];
        if (!strcasecmp(argv[0], cmd.name)) {
            if (argc != cmd.arity + 1) {
                add_reply(c, "-ERR wrong number of arguments");
                return;
            }
            (*cmd.func)(c, argv+1);
            return;
        }
    }

    add_reply(c, "-ERR unknown command %s", argv[0]);
}
