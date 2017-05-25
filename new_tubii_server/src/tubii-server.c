#include "tubii_client.h"
#include "ae.h"
#include "anet.h"
#include "db.h"
#include <errno.h>
#include <stdio.h>
#include "data.h"
#include <signal.h>
#include "logging.h"
#include <stdlib.h>
#include "server.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

aeEventLoop *el;
database *detector_db;

static struct config {
    int daemonize;
    char *logserver;
    char *dataserver;
    char *logfile;
    int loglevel;
} config;
void auto_load_config(char* file);

struct DBconfig dbconfig;

/*------------------------------------------------------------------------------
 * User interface
 *--------------------------------------------------------------------------- */

static void usage(void) {
    fprintf(stderr,
"tubii-server"
"\n"
"  -d                    daemonize\n"
"  --log-server <host>   Log server hostname (default: 'minard').\n"
"  --data-server <host>  Data server hostname (default: 'daq1').\n"
"  --logfile <filename>  Filename for log file.\n"
"  -v                    Increase verbosity (default: NOTICE).\\n).\n"
"  -q                    Decrease verbosity (default: NOTICE).\\n).\n"
"  --help                Output this help and exit.\n"
"\n");
    exit(1);
}

static int parseOptions(int argc, char **argv)
{
    int i;

    for (i = 1; i < argc; i++) {
        int lastarg = i == argc - 1;

        if (!strcmp(argv[i], "-h") && lastarg) {
            usage();
        } else if (!strcmp(argv[i], "--help")) {
            usage();
        } else if (!strcmp(argv[i],"-d")) {
            config.daemonize = 1;
        } else if (!strcmp(argv[i], "--log-server") && !lastarg) {
            config.logserver = argv[++i];
        } else if (!strcmp(argv[i], "--data-server") && !lastarg) {
            config.dataserver = argv[++i];
        } else if (!strcmp(argv[i],"--logfile") && !lastarg) {
            config.logfile = argv[++i];
        } else if (!strcmp(argv[i],"--config") && !lastarg){
        	printf("%s\n", argv[++i]);
        	FILE *fp=fopen(argv[i],"r");
        	if(fp!=0){
            	char call[255];
            	while(fscanf(fp,"%s",call)!=EOF){
            		if(strcmp(call,"user")==0) fscanf(fp,"%s",dbconfig.user);//=response;
            		else if(strcmp(call,"pass")==0) fscanf(fp,"%s",dbconfig.password);//=response;
            		else if(strcmp(call,"dbname")==0) fscanf(fp,"%s",dbconfig.name);//=response;
            		else if(strcmp(call,"dbhost")==0) fscanf(fp,"%s",dbconfig.host);//=response;
            		else printf("TUBii: Unrecognised data type in config file, %s\n\n",call);//, response);
            	}
        	}
        	fclose(fp);
        } else if (!strcmp(argv[i],"-v")) {
            config.loglevel--;
        } else if (!strcmp(argv[i],"-q")) {
            config.loglevel++;
        } else {
            if (argv[i][0] == '-') {
                fprintf(stderr,
                     "Unrecognized option or bad number of args for '%s'\n",
                     argv[i]);
                exit(1);
            } else {
                break;
            }
        }
    }
    return i;
}

struct command commandTable[] = {
		// clocks
		{"clockReset",   clockreset,   1},
		{"clockStatus",  clockstatus,  1},
		// shift registers
		{"dataready",    dataready,    2},
		{"loadShift",  	 loadShift,    2},
		{"muxenable",    muxenable,    2},
		{"muxer",   	 muxer,        2},
		// utilities
		{"initialise",	 initialise,   1},
		{"MZHappy",      MZHappy,      1},
		{"setMZHappyPulser", SetMZHappyPulser, 4},
		{"ping",         ping,         1},
		// ellie
		{"setGenericDelay",  	SetGenericdelay,  	2},
		{"setGenericPulser", 	SetGenericpulser, 	4},
		{"getGenericRate", 		GetPulserRate, 		1},
		{"getGenericPulseWidth", GetPulserWidth, 	1},
		{"getGenericNPulses", 	GetPulserNPulses, 	1},
		{"getGenericDelay", 	GetDelay, 			1},
		{"lengthenDelay", 		LengthenDelay, 		2},
		{"setSmellieDelay", 	SetSmelliedelay, 	2},
		{"setSmelliePulser",	SetSmelliepulser,	4},
		{"getSmellieRate", 		GetSmellieRate, 	1},
		{"getSmelliePulseWidth", GetSmelliePulseWidth, 1},
		{"getSmellieNPulses", 	GetSmellieNPulses, 	1},
		{"getSmellieDelay", 	GetSmellieDelay, 	1},
		{"setTellieDelay",  	SetTelliedelay,  	2},
		{"setTelliePulser", 	SetTelliepulser, 	4},
		{"getTellieRate", 		GetTellieRate, 		1},
		{"getTelliePulseWidth", GetTelliePulseWidth, 1},
		{"getTellieNPulses", 	GetTellieNPulses, 	1},
		{"getTellieDelay", 		GetTellieDelay, 	1},
		{"STOP", 				StopTUBii, 			1},
		{"keepAlive", 			KeepAlive, 			1},
		// triggers
		{"setCounterMask", 	SetCounterMask, 2},
		{"getCounterMask", 	GetCounterMask, 1},
		{"setSpeakerMask", 	SetSpeakerMask, 2},
		{"getSpeakerMask", 	GetSpeakerMask, 1},
		{"setSpeakerScale", SetSpeakerScale, 2},
		{"setTriggerMask", 	SetTriggerMask, 3},
		{"getSyncTriggerMask", 	GetSyncTriggerMask, 1},
		{"getAsyncTriggerMask", 	GetAsyncTriggerMask, 1},
		{"softGT", 		   	SoftGT, 		1},
		{"countLatch",     	countLatch,   	2},
		{"countReset",     	countReset,   	2},
		{"countMode",      	countMode,    	2},
		{"gtdelay",        	gtdelay,      	2},
		{"settrigworddelay", 	SetTrigWordDelay, 	2},
		{"startReadout",   		start_data_readout, 1},
		{"stopReadout",	   		stop_data_readout,  1},
		{"setBurstTrigger",	    SetBurstTrigger,    4},
		{"setTUBiiPGT",         SetTUBiiPGT,        2},
		{"getTUBiiPGT",         GetTUBiiPGT,        1},
		{"setComboTrigger",    	SetComboTrigger,    3},
		{"setPrescaleTrigger", 	SetPrescaleTrigger, 3},
		{"GetTrigger",  		GetTrigger,  1},
		{"GetGTID", 			GetGTID,		1},
		{"GetFifoTrigger",     	GetFifoTrigger,     1},
		{"ResetFifo",     	   	ResetFIFO,	   	    1},
		{"ResetGTID",		   	ResetGTID,          1},
		/// High level functions
		{"setGTDelays", 	SetGTDelays, 	3},
		{"getLODelay", 		GetLODelay, 	1},
		{"getDGTDelay", 	GetDGTDelay, 	1},
		{"setCAENWords", 	SetCaenWords, 	3},
		{"getCAENGainPathWord", 		GetCAENGainPathWord, 	  1},
		{"getCAENChannelSelectWord", 	GetCAENChannelSelectWord, 1},
		{"setControlReg", 	SetControlReg, 	 2},
		{"getControlReg", 	GetControlReg, 	 1},
		{"setECalBit", 		SetECalBit, 	 2},
		{"setDACThreshold", SetDACThreshold, 2},
		{"getDACThreshold", GetDACThreshold, 1},
		{"setAllowableClockMisses", SetAllowableClockMisses, 2},
		// XADC
		{"xadc", xadc, 1},
		//DB
		{"save", save_TUBii_command, 1},
		{"load", load_TUBii_command, 2},
		{"loadConfig", load_new_config, 2}
};

void sigint_handler(int dummy)
{
    el->stop = 1;
}

void daemonize(void) {
    int fd;

    if (fork() != 0) exit(0); /* parent exits */
    setsid(); /* create a new session */

    /* Every output goes to /dev/null. If Redis is daemonized but
     * the 'logfile' is set to 'stdout' in the configuration file
     * it will not log at all. */
    if ((fd = open("/dev/null", O_RDWR, 0)) != -1) {
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        if (fd > STDERR_FILENO) close(fd);
    }
}



int main(int argc, char **argv)
{
    config.daemonize = 0;
    config.logserver = "minard";
    config.dataserver = "192.168.80.100";//"192.168.80.1";
    config.logfile = "";
    config.loglevel = NOTICE;

    parseOptions(argc, argv);

    strcpy(logfile, config.logfile);
    verbosity = config.loglevel;

    if (config.daemonize) daemonize();

    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, sigint_handler);

    Log(WARNING, "tubii server started");

    el = aeCreateEventLoop(100);

    if ((aeCreateTimeEvent(el, 0, printSkipped, NULL, NULL)) == AE_ERR) {
        LogRaw(WARNING, "failed to set up printSkipped()");
    }

    startLogServer(config.logserver, "tubii");

    initServer(el, 4001, commandTable, sizeof(commandTable)/sizeof(struct command));

    /* set up the dispatch_connect event which will try to connect to the
     * data stream server. If it can't connect, it will retry every 10
     * seconds. */
    if (data_connect(config.dataserver)) {
        Log(WARNING, "failed to set up data stream");
        return 1;
    }

    /* Set up the database connection */
    auto_load_config("/mnt/settings.cfg");
    detector_db = db_connect(el, dbconfig.host, dbconfig.name, dbconfig.user, dbconfig.password);
    if( detector_db == 0) {
         Log(WARNING, "failed to set up database connection");
         return 1;
    }

    /* Run TUBii's initialisation */
    auto_init();

    /* start tubii readout */
    if (start_tubii_readout(1000)) {
        //Log(WARNING, tubii_err);
        return 1;
    }

    /* check for ORCA connection */
    if ((aeCreateTimeEvent(el, 30000, daq_connection, NULL, NULL)) == AE_ERR) {
        Log(WARNING, "failed to check tubii's daq connection");
        return 1;
    }

    /* set up status event */
    if (aeCreateTimeEvent(el, 0, tubii_status, NULL, NULL) == AE_ERR) {
        Log(WARNING, "failed to set up status tubii");
        return 1;
    }

    /* enter the main event loop */
    el->stop = 0;
    while (!el->stop) {
        if (el->beforesleep != NULL)
            el->beforesleep(el);
        aeProcessEvents(el, AE_ALL_EVENTS);
    }

    Log(NOTICE, "ctrl-c caught. flushing buffers...");

    time_t now = time(NULL);
    while (time(NULL) < now + 1) {
        if (aeProcessEvents(el, AE_FILE_EVENTS | AE_DONT_WAIT) == 0) break;
    }

    aeDeleteEventLoop(el);

    return 0;
}
