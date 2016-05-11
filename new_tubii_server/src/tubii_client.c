#include "tubii_client.h"
#include "record_info.h"
#include "data.h"
#include "logging.h"
#include <stdlib.h>
#include <arpa/inet.h>
#include <errno.h>
#include "ae.h"
#include "server.h"

// tubii headers
#include "tubiiAddresses.h"
#include "tubiiClocks.h"
#include "tubiiELLIE.h"
#include "tubiiRegs.h"
#include "tubiiTriggers.h"
#include "tubiiUtil.h"
#include "tubiiXadc.h"

#include "libpq-fe.h"

extern aeEventLoop *el;
long long tubii_readout_id = AE_ERR;
int last_gtid=0;

// Initialisation functions
void initialise(client *c, int argc, sds *argv)
{
  // Calls auto_init which:
  //  -Maps all the memory
  //  -Sets registers
  auto_init();

  // Need to check for errors
  addReplyStatus(c, "+OK");
}

int InitMapping()
{
  // Ian: Do all the memory mappings on start up as it'll save time during each function call

  // Shift Registers
  MappedRegsBaseAddress= MemoryMapping(SHIFTREGS_BASEADDR,SHIFTREGS_HIGHADDR);

  // Clocks
  MappedClocksBaseAddress= MemoryMapping(CLOCKLOGIC_BASEADDR,CLOCKLOGIC_HIGHADDR);

  // Counter Latch & Reset
  MappedCountBaseAddress= MemoryMapping(COUNTDISP_BASEADDR,COUNTDISP_HIGHADDR);
  MappedCountLengthenBaseAddress= MemoryMapping(COUNTLENGTHEN_BASEADDR,COUNTLENGTHEN_HIGHADDR);

  // Trigger, Counter & Speaker Masks
  MappedTrigBaseAddress= MemoryMapping(TRIGGEROUT_BASEADDR,TRIGGEROUT_HIGHADDR);
  MappedGTIDBaseAddress= MemoryMapping(GTID_BASEADDR,GTID_HIGHADDR);
  MappedFifoBaseAddress= MemoryMapping(FIFOREADOUT_BASEADDR,FIFOREADOUT_HIGHADDR);

  // Meta-Triggers
  MappedBurstBaseAddress= MemoryMapping(BURSTTRIG_BASEADDR,BURSTTRIG_HIGHADDR);
  MappedButtonBaseAddress= MemoryMapping(BUTTONTRIG_BASEADDR,BUTTONTRIG_HIGHADDR);
  MappedComboBaseAddress= MemoryMapping(COMBOTRIG_BASEADDR,COMBOTRIG_HIGHADDR);
  MappedPrescaleBaseAddress= MemoryMapping(PRESCALETRIG_BASEADDR,PRESCALETRIG_HIGHADDR);

  // Pulsers
  MappedPulserBaseAddress= MemoryMapping(GENERICPULSER_BASEADDR,GENERICPULSER_HIGHADDR);
  MappedSPulserBaseAddress= MemoryMapping(SMELLIEPULSER_BASEADDR,SMELLIEPULSER_HIGHADDR);
  MappedTPulserBaseAddress= MemoryMapping(TELLIEPULSER_BASEADDR,TELLIEPULSER_HIGHADDR);
  MappedHappyBaseAddress= MemoryMapping(MZHAPPY_BASEADDR,MZHAPPY_HIGHADDR);

  // Delays
  MappedDelayBaseAddress= MemoryMapping(GENERICDELAY_BASEADDR,GENERICDELAY_HIGHADDR);
  MappedDelayLengthenBaseAddress= MemoryMapping(DELAYLENGTHEN_BASEADDR,DELAYLENGTHEN_HIGHADDR);
  MappedSDelayBaseAddress= MemoryMapping(SMELLIEDELAY_BASEADDR,SMELLIEDELAY_HIGHADDR);
  MappedTDelayBaseAddress= MemoryMapping(TELLIEDELAY_BASEADDR,TELLIEDELAY_HIGHADDR);
  MappedGTDelayBaseAddress= MemoryMapping(GTDELAY_BASEADDR,GTDELAY_HIGHADDR);
  MappedTrigWordDelayBaseAddress= MemoryMapping(TRIGWORDDELAY_BASEADDR,TRIGWORDDELAY_HIGHADDR);

  // XADC
  MappedXADCBaseAddress= MemoryMapping(XADC_BASEADDR,XADC_HIGHADDR);

  return 0;
}

int auto_init()
{
  // Map the registers
  InitMapping();

  // MZHappy
  Pulser(1,0.5,1e9,MappedHappyBaseAddress);

  // Reset the FIFO
  resetFIFO();

  //Put caen in attenuating mode
  CAENWords(255, 255);
  //setup DGT and LO* delay lengths
  GTDelays(153, 153);
  //Set MTCA MIMIC DAC value
  DACThresholds(4095);
  //Set Control Reg Value
  ControlReg(58);

  return 0;
}


// Clock commands
void clockreset(client *c, int argc, sds *argv)
{
  int ret= clockReset(1);
  usleep(1000);
  ret= clockReset(0);

  if(ret != 0) addReplyError(c, tubii_err);
  else addReplyStatus(c, "+OK");
}

void clockstatus(client *c, int argc, sds *argv)
{
  int status= clockStatus();

  // do something with ret which is the status of the backup clock
  // add to the datastream?

  addReply(c, ":%d", status);
}

// Utility commands
void MZHappy(client *c, int argc, sds *argv)
{
  // Set for 1e9 pulses. Should renew this in the status readout.
  int ret= Pulser(1,0.5,1e9,MappedHappyBaseAddress);

  if(ret == 0) addReplyStatus(c, "+OK");
  else addReplyError(c, tubii_err);
}

void SetMZHappyPulser(client *c, int argc, sds *argv)
{
  // For Ian's debugging purposes, MZHappy can also be used as a pulser
  float rate=0, length=0;
  uint32_t nPulse=0;
  safe_strtof(argv[1],&rate);
  safe_strtof(argv[2],&length);
  safe_strtoul(argv[3],&nPulse);

  int ret= Pulser(rate,length,nPulse,MappedHappyBaseAddress);

  if(ret == 0) addReplyStatus(c, "+OK");
  else addReplyError(c, tubii_err);
}

void ping(client *c, int argc, sds *argv)
{
  addReplyStatus(c, "+PING");
}

// ELLIE commands
void SetGenericpulser(client *c, int argc, sds *argv)
{
  // Need to sync. create pulse and delay it async.
  float rate=0, length=0;
  uint32_t nPulse=0;
  safe_strtof(argv[1],&rate);
  safe_strtof(argv[2],&length);
  safe_strtoul(argv[3],&nPulse);

  // 1. Create Pulse
  int ret1= Pulser(rate,length,nPulse,MappedPulserBaseAddress);
  if(ret1!=0){
    addReplyError(c, tubii_err);
    return;
  }

  // 2. Delay Pulse
  /*int ret2= Muxer("4");
  int ret3= LoadShift(argv[1]);
  */
  addReplyStatus(c, "+OK");
}

void SetGenericdelay(client *c, int argc, sds *argv)
{
  // 1. Need to split argument into the sync part and async part
  // Something like multiples of 10 go to sync and ones go to async
  float length=0;
  safe_strtof(argv[1],&length);
  u32 delay = length*ns;

  // 2. Sync part
  int ret1= Delay(delay,MappedDelayBaseAddress);
  if(ret1 != 0){
    addReplyError(c, tubii_err);
    return;
  }

  // 3. Async part to shift reg:
  //int ret2= Muxer("5");
  //int ret3= LoadShift(argv[1]);

  addReplyStatus(c, "+OK");
}

void LengthenDelay(client *c, int argc, sds *argv)
{
	int ret= Lengthen(argv[1]);

	if(ret==0) addReplyStatus(c, "+OK");
	else addReplyError(c, tubii_err);
}

void SetSmelliepulser(client *c, int argc, sds *argv)
{
  float rate=0, length=0;
  uint32_t nPulse=0;
  safe_strtof(argv[1],&rate);
  safe_strtof(argv[2],&length);
  safe_strtoul(argv[3],&nPulse);

  int ret= Pulser(rate,length,nPulse,MappedSPulserBaseAddress);

  if(ret == 0) addReplyStatus(c, "+OK");
  else addReplyError(c, tubii_err);
}

void SetSmelliedelay(client *c, int argc, sds *argv)
{
  float length=0;
  safe_strtof(argv[1],&length);
  u32 delay = length*ns;
  int ret= Delay(delay,MappedSDelayBaseAddress);

  if(ret == 0) addReplyStatus(c, "+OK");
  else addReplyError(c, tubii_err);
}

void SetTelliepulser(client *c, int argc, sds *argv)
{
  float rate=0, length=0;
  uint32_t nPulse=0;
  safe_strtof(argv[1],&rate);
  safe_strtof(argv[2],&length);
  safe_strtoul(argv[3],&nPulse);

  int ret= Pulser(rate,length,nPulse,MappedTPulserBaseAddress);

  if(ret == 0) addReplyStatus(c, "+OK");
  else addReplyError(c, tubii_err);;
}

void SetTelliedelay(client *c, int argc, sds *argv)
{
  float length=0;
  safe_strtof(argv[1],&length);
  u32 delay = length*ns;
  int ret= Delay(delay,MappedTDelayBaseAddress);

  if(ret == 0) addReplyStatus(c, "+OK");
  else addReplyError(c, tubii_err);
}

void GetSmellieRate(client *c, int argc, sds *argv)
{
  u32 period= mReadReg((u32) MappedSPulserBaseAddress, RegOffset1);
  float rate= HunMHz/period;

  addReply(c, ":%d", rate);
}

void GetSmelliePulseWidth(client *c, int argc, sds *argv)
{
  float width= (mReadReg((u32) MappedSPulserBaseAddress, RegOffset1) - mReadReg((u32) MappedSPulserBaseAddress, RegOffset0))*HunMHz;
  addReply(c, ":%d", width);
}

void GetSmellieNPulses(client *c, int argc, sds *argv)
{
  addReply(c, ":%d", mReadReg((u32) MappedSPulserBaseAddress, RegOffset3));
}

void GetSmellieDelay(client *c, int argc, sds *argv)
{
  addReply(c, ":%d", mReadReg((u32) MappedSDelayBaseAddress, RegOffset3));
}

void GetTellieRate(client *c, int argc, sds *argv)
{
  u32 period= mReadReg((u32) MappedTPulserBaseAddress, RegOffset1);
  float rate= HunMHz/period;

  addReply(c, ":%d", rate);
}

void GetTelliePulseWidth(client *c, int argc, sds *argv)
{
  float width= (mReadReg((u32) MappedTPulserBaseAddress, RegOffset1) - mReadReg((u32) MappedTPulserBaseAddress, RegOffset0))*HunMHz;
  addReply(c, ":%d", width);
}

void GetTellieNPulses(client *c, int argc, sds *argv)
{
  addReply(c, ":%d", mReadReg((u32) MappedTPulserBaseAddress, RegOffset3));
}

void GetTellieDelay(client *c, int argc, sds *argv)
{
  addReply(c, ":%d", mReadReg((u32) MappedTDelayBaseAddress, RegOffset3));
}

void GetPulserRate(client *c, int argc, sds *argv)
{
  u32 period= mReadReg((u32) MappedPulserBaseAddress, RegOffset1);
  float rate= HunMHz/period;

  addReply(c, ":%d", rate);
}

void GetPulserWidth(client *c, int argc, sds *argv)
{
  float width= (mReadReg((u32) MappedPulserBaseAddress, RegOffset1) - mReadReg((u32) MappedPulserBaseAddress, RegOffset0))*HunMHz;
  addReply(c, ":%d", width);
}

void GetPulserNPulses(client *c, int argc, sds *argv)
{
  addReply(c, ":%d", mReadReg((u32) MappedPulserBaseAddress, RegOffset3));
}

void GetDelay(client *c, int argc, sds *argv)
{
  addReply(c, ":%d", mReadReg((u32) MappedDelayBaseAddress, RegOffset3));
}

//// Shift Register commands
//   Low level stuff
void dataready(client *c, int argc, sds *argv)
{
  uint32_t dReady;
  safe_strtoul(argv[1],&dReady);
  int ret= DataReady(dReady);

  if(ret==0) addReplyStatus(c, "+OK");
  else addReplyError(c, tubii_err);
}

void loadShift(client *c, int argc, sds *argv)
{
  uint32_t lShift;
  safe_strtoul(argv[1],&lShift);
  int ret= LoadShift(lShift);

  if(ret == 0) addReplyStatus(c, "+OK");
  else addReplyError(c, tubii_err);
}

void muxenable(client *c, int argc, sds *argv)
{
  uint32_t muxEn;
  safe_strtoul(argv[1],&muxEn);
  int ret= MuxEnable(muxEn);

  if(ret == 0) addReplyStatus(c, "+OK");
  else addReplyError(c, tubii_err);
}

void muxer(client *c, int argc, sds *argv)
{
  uint32_t mux;
  safe_strtoul(argv[1],&mux);
  int ret= Muxer(mux);

  if(ret == 0) addReplyStatus(c, "+OK");
  else addReplyError(c, tubii_err);
}

// Control register
void SetControlReg(client *c, int argc, sds *argv)
{
  uint32_t cReg;
  safe_strtoul(argv[1],&cReg);
  ControlReg(cReg);
  addReplyStatus(c, "+OK");
}

void GetControlReg(client *c, int argc, sds *argv)
{
  // This won't be done by ReadShift due to a bug in the hardware
  addReply(c, ":%d", mReadReg((u32) MappedRegsBaseAddress, RegOffset10));
}

void SetECalBit(client *c, int argc, sds *argv)
{
  uint32_t cReg;
  safe_strtoul(argv[1],&cReg);
  if(cReg==1 || cReg==0){
	  ControlReg((mReadReg((u32) MappedRegsBaseAddress, RegOffset10) & 4294967291) + 4*cReg);
	  addReplyStatus(c, "+OK");
  }
  else{
	  addReplyError(c, "ECals can only be set on (1) or off (0).");
  }
}

// CAEN Settings
void SetCaenWords(client *c, int argc, sds *argv)
{
  uint32_t gPath, cSelect;
  safe_strtoul(argv[1],&gPath);
  safe_strtoul(argv[2],&cSelect);
  CAENWords(gPath, cSelect);
  addReplyStatus(c, "+OK");
}

void GetCAENGainPathWord(client *c, int argc, sds *argv)
{
  addReply(c, ":%d", mReadReg((u32) MappedRegsBaseAddress, RegOffset11));
}

void GetCAENChannelSelectWord(client *c, int argc, sds *argv)
{
  addReply(c, ":%d", mReadReg((u32) MappedRegsBaseAddress, RegOffset12));
}

// DAC Settings
void SetDACThreshold(client *c, int argc, sds *argv)
{
  uint32_t dacThresh;
  safe_strtoul(argv[1],&dacThresh);
  DACThresholds(dacThresh);
  addReplyStatus(c, "+OK");
}

void GetDACThreshold(client *c, int argc, sds *argv)
{
  addReply(c, ":%d", mReadReg((u32) MappedRegsBaseAddress, RegOffset13));
}

// DGT & LO
void SetGTDelays(client *c, int argc, sds *argv)
{
  uint32_t loDelay, dgtDelay;
  safe_strtoul(argv[1],&loDelay);
  safe_strtoul(argv[2],&dgtDelay);
  GTDelays(loDelay, dgtDelay);
  addReplyStatus(c, "+OK");
}

void GetLODelay(client *c, int argc, sds *argv)
{
  addReply(c, ":%d", mReadReg((u32) MappedRegsBaseAddress, RegOffset14));
}

void GetDGTDelay(client *c, int argc, sds *argv)
{
  addReply(c, ":%d", mReadReg((u32) MappedRegsBaseAddress, RegOffset15));
}

void SetAllowableClockMisses(client *c, int argc, sds *argv)
{
  uint32_t nMisses;
  safe_strtoul(argv[1],&nMisses);
  ClockMisses(nMisses);
  addReplyStatus(c, "+OK");
}


// Trigger Commands
void countLatch(client *c, int argc, sds *argv)
{
  uint32_t latch;
  safe_strtoul(argv[1],&latch);
  counterLatch(latch);
  addReplyStatus(c, "+OK");
}

void countReset(client *c, int argc, sds *argv)
{
  uint32_t reset;
  safe_strtoul(argv[1],&reset);
  counterReset(reset);
  addReplyStatus(c, "+OK");
}

void countMode(client *c, int argc, sds *argv)
{
  uint32_t mode;
  safe_strtoul(argv[1],&mode);
  counterMode(mode);
  addReplyStatus(c, "+OK");
}

void SetCounterMask(client *c, int argc, sds *argv)
{
  uint32_t mask;
  safe_strtoul(argv[1],&mask);
  counterMask(mask);
  addReplyStatus(c, "+OK");
}

void GetCounterMask(client *c, int argc, sds *argv)
{
  addReply(c, ":%u", getCounterMask());
}

void SetSpeakerMask(client *c, int argc, sds *argv)
{
  uint32_t mask;
  safe_strtoul(argv[1],&mask);
  speakerMask(mask);
  addReplyStatus(c, "+OK");
}

void GetSpeakerMask(client *c, int argc, sds *argv)
{
  addReply(c, ":%u", getSpeakerMask());
}

void SetTriggerMask(client *c, int argc, sds *argv)
{
  triggerMask(argv[1]);
  addReplyStatus(c, "+OK");
}

void GetTriggerMask(client *c, int argc, sds *argv)
{
  addReply(c, ":%u", getTriggerMask());
}

void SetBurstTrigger(client *c, int argc, sds *argv)
{
  float rate;
  uint32_t bit;
  safe_strtof(argv[1],&rate);
  safe_strtoul(argv[2],&bit);

  if(burstTrig(rate,bit) == 0) addReplyStatus(c, "+OK");
  else addReplyError(c, tubii_err);
}

void SetButtonTrigger(client *c, int argc, sds *argv)
{
  buttonTrig();
  addReplyStatus(c, "+OK");
}

void SetComboTrigger(client *c, int argc, sds *argv)
{
  u32 enableMask, logicMask;
  safe_strtoul(argv[1],&enableMask);
  safe_strtoul(argv[2],&logicMask);

  if(comboTrig(enableMask,logicMask) == 0) addReplyStatus(c, "+OK");
  else addReplyError(c, tubii_err);
}

void SetPrescaleTrigger(client *c, int argc, sds *argv)
{
  float rate;
  uint32_t bit;
  safe_strtof(argv[1],&rate);
  safe_strtoul(argv[2],&bit);

  if(prescaleTrig(rate,bit) == 0) addReplyStatus(c, "+OK");
  else addReplyError(c, tubii_err);
}

void SetTrigWordDelay(client *c, int argc, sds *argv)
{
  float length=0;
  safe_strtof(argv[1],&length);
  u32 delay = length*ns;

  int ret= TrigWordDelay(delay);
  if(ret!=0){
	addReplyError(c, tubii_err);
	return;
  }
  addReplyStatus(c, "+OK");
}

void ResetGTID(client *c, int argc, sds *argv)
{
  resetGTID();
  addReplyStatus(c, "+OK");
}

void SoftGT(client *c, int argc, sds *argv)
{
  softGT();
  addReplyStatus(c, "+OK");
}

void ResetFIFO(client *c, int argc, sds *argv)
{
  resetFIFO();
  addReplyStatus(c, "+OK");
}

void gtdelay(client *c, int argc, sds *argv)
{
  float length=0;
  safe_strtof(argv[1],&length);
  u32 delay = length*ns;
  int ret= Delay(delay,MappedGTDelayBaseAddress);

  if(ret == 0) addReplyStatus(c, "+OK");
  else addReplyError(c, tubii_err);
}

// Data readout
void GetCurrentTrigger(client *c, int argc, sds *argv)
{
  addReply(c, ":%u", currentTrigger());
}

void GetFifoTrigger(client *c, int argc, sds *argv)
{
    struct MegaRecord mega;
	struct GenericRecordHeader header;

    mega.size=0;
    int loop=0;
	while(loop<1000){ //empty==0){
	//usleep(100);
    loop++;
	/* Send a TUBii_RECORD to the data stream server */
    struct TubiiRecord trec;
    fifoTrigger(&trec);

    printf("GTID %i Word %i\n",trec.GTID,trec.TrigWord);
    if(last_gtid!=trec.GTID){
    if(last_gtid!=trec.GTID-1 && trec.GTID!=0) printf("Missed one! %i --> %i\n",last_gtid,trec.GTID);

    last_gtid=trec.GTID;
    printf("GTID %i Word %i\n",trec.GTID,trec.TrigWord);

    mega.array[mega.size]=trec;
    mega.size = mega.size+1;
    }
	}

	if(mega.size>0){
    printf("Bundle!\n");
    printf("%i events!\n",mega.size);

    header.RecordID = htonl(MEGA_RECORD);
    header.RecordLength = htonl(sizeof(u32)*(2*mega.size+1));
    header.RecordVersion = htonl(RECORD_VERSION);

    write_to_data_stream(&header, &mega);
    }

    addReplyStatus(c, "+OK");
}

int tubii_status(aeEventLoop *el, long long id, void *data)
{
    // Check if we are in rate mode
	if(counter_mode == 1){
	  // Toggle latchs
	  GetRate();
	}

	// Renew the MZHappy Pulser
	Pulser(1,0.5,100,MappedHappyBaseAddress);

	/* Sends TUBii status record to the data stream */
    struct GenericRecordHeader header;
    struct TubiiStatus status;
    status.Clock = clockStatus();
    status.GTID_out = last_gtid;
    status.GTID_in = currentgtid();
    status.FIFO = fifoStatus();

    header.RecordID = htonl(TUBII_STATUS);
    header.RecordLength = htonl(sizeof(status));
    header.RecordVersion = htonl(RECORD_VERSION);

    write_to_data_stream(&header, &status);

    return 1000; // run again in one second
    /* or, if you are done with this event */
    //return AE_NOMORE;
}


void start_data_readout(client *c, int argc, sds *argv)
{
	data_readout=1;
	addReplyStatus(c, "+OK");
}

void stop_data_readout(client *c, int argc, sds *argv)
{
	data_readout=0;
	addReplyStatus(c, "+OK");
}

int getDataReadout()
{
	return data_readout;
}

int start_tubii_readout(long long milliseconds)
{
    if (tubii_readout_id != AE_ERR) {
        sprintf(tubii_err, "TUBii: readout already running!");
        return -1;
    }
    //if(getDataReadout() == 0) return 0;

    // set up read out event
    if ((tubii_readout_id = aeCreateTimeEvent(el, milliseconds, tubii_readout, NULL, NULL)) == AE_ERR) {
        sprintf(tubii_err, "failed to set up tubii readout");
        return -1;
    }

    return 0;
}

int tubii_readout(aeEventLoop *el, long long id, void *data)
{
	// Check if we want to read data
    if(getDataReadout() == 0) return 1000;

	struct MegaRecord mega;
	struct GenericRecordHeader header;

    mega.size=0;
    int loop=0;
    struct TubiiRecord trec;
	for(loop=0; loop<1000; loop++){
		/* Send a TUBii_RECORD to the data stream server */
		fifoTrigger(&trec);

		// Have we advanced?
		if(last_gtid!=trec.GTID){
			// Have we missed a tick
			if(last_gtid!=trec.GTID-1 && trec.GTID!=0 && trec.GTID!=1 && (last_gtid & 0xFFFF)!=0xFFFE)
				Log(WARNING, "TUBII: FIFO skipped a GTID from %i to %i\n",last_gtid,trec.GTID);

			last_gtid=trec.GTID;

			mega.array[mega.size]=trec;
			mega.size = mega.size+1;
		}
	}

	if(mega.size>0){
		//printf("Bundle! %i events!\n",mega.size);
		header.RecordID = htonl(MEGA_RECORD);
		header.RecordLength = htonl(sizeof(mega));
		header.RecordVersion = htonl(RECORD_VERSION);

		write_to_data_stream(&header, &mega);
    }

    return 1;
}

void PostGresNonsense()
{
	const char *conninfo = "dbname = postgres";
	PGconn     *conn;
	PGresult   *res;
	int         nFields, i, j;

	/* Make a connection to the database */
	conn = PQconnectdb(conninfo);

	/* Check to see that the backend connection was successfully made */
	if (PQstatus(conn) != CONNECTION_OK)
	{
		printf("Connection to database failed: %s", PQerrorMessage(conn));
	    PQfinish(conn);
	    exit(1);
	}

	/* Our test case here involves using a cursor, for which we must be inside
	 * a transaction block.  We could do the whole thing with a single
	 * PQexec() of "select * from pg_database", but that's too trivial to make
	 * a good example. */

	/* Start a transaction block */
	res = PQexec(conn, "BEGIN");
	if (PQresultStatus(res) != PGRES_COMMAND_OK)
	{
		printf("BEGIN command failed: %s", PQerrorMessage(conn));
		PQclear(res);
		PQfinish(conn);
		exit(1);
	}

	/* Should PQclear PGresult whenever it is no longer needed to avoid memory leaks */
	PQclear(res);

	/* Fetch rows from pg_database, the system catalog of databases */
	res = PQexec(conn, "DECLARE myportal CURSOR FOR select * from pg_database");
	if (PQresultStatus(res) != PGRES_COMMAND_OK)
	{
		printf("DECLARE CURSOR failed: %s", PQerrorMessage(conn));
		PQclear(res);
	    PQfinish(conn);
	    exit(1);
	}
	PQclear(res);

	res = PQexec(conn, "FETCH ALL in myportal");
	if (PQresultStatus(res) != PGRES_TUPLES_OK)
	{
		printf("FETCH ALL failed: %s", PQerrorMessage(conn));
	    PQclear(res);
	    PQfinish(conn);
	    exit(1);
	}

	/* first, print out the attribute names */
	nFields = PQnfields(res);
	for (i = 0; i < nFields; i++)
		printf("%-15s", PQfname(res, i));
	printf("\n\n");

	/* next, print out the rows */
	for (i = 0; i < PQntuples(res); i++)
	{
		for (j = 0; j < nFields; j++)
			printf("%-15s", PQgetvalue(res, i, j));
	    printf("\n");
	}

	PQclear(res);

	/* close the portal ... we don't bother to check for errors ... */
	res = PQexec(conn, "CLOSE myportal");
	PQclear(res);

	/* end the transaction */
	res = PQexec(conn, "END");
	PQclear(res);

	/* close the connection to the database and cleanup */
	PQfinish(conn);

	return 0;
}

static XAdcPs XADCMonInst;

void xadc(client *c, int argc, sds *argv)
{
	XAdcPs_Config *ConfigPtr;
	XAdcPs *XADCInstPtr = &XADCMonInst;

	//status of initialisation
	int Status_ADC;

	//temperature readings
	u32 TempRawData;
	float TempData;

	//Vcc Int readings
	u32 VccIntRawData;
	float VccIntData;

	//Vcc Aux readings
	u32 VccAuxRawData;
	float VccAuxData;

	//Vbram readings
	u32 VBramRawData;
	float VBramData;

	//VccPInt readings
	u32 VccPIntRawData;
	float VccPIntData;

	//VccPAux readings
	u32 VccPAuxRawData;
	float VccPAuxData;

	//Vddr readings
	u32 VDDRRawData;
	float VDDRData;

	printf("Start to do stuff\n");

    //XADC initilization
    ConfigPtr = XAdcPs_LookupConfig(XPAR_AXI_XADC_0_DEVICE_ID);
    if (ConfigPtr == NULL) {
      return XST_FAILURE;
    }

	printf("Initialised\n");
    Status_ADC = XAdcPs_CfgInitialize(XADCInstPtr,ConfigPtr,MappedXADCBaseAddress);
    if(XST_SUCCESS != Status_ADC){
      printf("ADC INIT FAILED\n\r");
      addReplyStatus(c, "+NOT OK");
    }

    /*printf("Self-test\n");
    //self test
    Status_ADC = XAdcPs_SelfTest(XADCInstPtr);
 	if (Status_ADC != XST_SUCCESS) {
 	  printf("Failed Self-test!\n");
 		addReplyStatus(c, "+NOT OK");
 	}*/

	printf("Set squencer\n");
 	//stop sequencer
 	XAdcPs_SetSequencerMode(XADCInstPtr,XADCPS_SEQ_MODE_SINGCHAN);

    //disable alarms
    XAdcPs_SetAlarmEnables(XADCInstPtr, 0x0);

    //configure sequencer to just sample internal on chip parameters
    XAdcPs_SetSeqInputMode(XADCInstPtr, XADCPS_SEQ_MODE_SAFE);

    //configure the channel enables we want to monitor
    XAdcPs_SetSeqChEnables(XADCInstPtr,XADCPS_CH_TEMP|XADCPS_CH_VCCINT|XADCPS_CH_VCCAUX|XADCPS_CH_VBRAM|XADCPS_CH_VCCPINT| XADCPS_CH_VCCPAUX|XADCPS_CH_VCCPDRO);

	printf("Begin Loop\n");
    /*while(1){
      TempRawData = XAdcPs_GetAdcData(XADCInstPtr, XADCPS_CH_TEMP);
      TempData = XAdcPs_RawToTemperature(TempRawData);
      printf("Raw Temp %lu Real Temp %f \n\r", TempRawData, TempData);

      VccIntRawData = XAdcPs_GetAdcData(XADCInstPtr, XADCPS_CH_VCCINT);
      VccIntData = XAdcPs_RawToVoltage(VccIntRawData);
      printf("Raw VccInt %lu Real VccInt %f \n\r", VccIntRawData, VccIntData);

      VccAuxRawData = XAdcPs_GetAdcData(XADCInstPtr, XADCPS_CH_VCCAUX);
      VccAuxData = XAdcPs_RawToVoltage(VccAuxRawData);
      printf("Raw VccAux %lu Real VccAux %f \n\r", VccAuxRawData, VccAuxData);

      //VrefPRawData = XAdcPs_GetAdcData(XADCInstPtr, XADCPS_CH_VREFP);
      //VrefPData = XAdcPs_RawToVoltage(VrefPRawData);
      //printf("Raw VRefP %lu Real VRefP %f \n\r", VrefPRawData, VrefPData);

      //VrefNRawData = XAdcPs_GetAdcData(XADCInstPtr, XADCPS_CH_VREFN);
      //VrefNData = XAdcPs_RawToVoltage(VrefNRawData);
      //printf("Raw VRefN %lu Real VRefN %f \n\r", VrefNRawData, VrefNData);

      VBramRawData = XAdcPs_GetAdcData(XADCInstPtr, XADCPS_CH_VBRAM);
      VBramData = XAdcPs_RawToVoltage(VBramRawData);
      printf("Raw VccBram %lu Real VccBram %f \n\r", VBramRawData, VBramData);

      VccPIntRawData = XAdcPs_GetAdcData(XADCInstPtr, XADCPS_CH_VCCPINT);
      VccPIntData = XAdcPs_RawToVoltage(VccPIntRawData);
      printf("Raw VccPInt %lu Real VccPInt %f \n\r", VccPIntRawData, VccPIntData);

      VccPAuxRawData = XAdcPs_GetAdcData(XADCInstPtr, XADCPS_CH_VCCPAUX);
      VccPAuxData = XAdcPs_RawToVoltage(VccPAuxRawData);
      printf("Raw VccPAux %lu Real VccPAux %f \n\r", VccPAuxRawData, VccPAuxData);

      VDDRRawData = XAdcPs_GetAdcData(XADCInstPtr, XADCPS_CH_VCCPDRO);
      VDDRData = XAdcPs_RawToVoltage(VDDRRawData);
      printf("Raw VccDDR %lu Real VccDDR %f \n\r", VDDRRawData, VDDRData);
    }*/

	addReplyStatus(c, "+OK");
}



int safe_strtof(char *s, float *f)
{
    /* Convert string s -> float. Returns -1 on error. */
    char *end;

    errno = 0;
    *f = (float) strtod(s, &end);

    if (end == s) {
        return -1;
    } else if ('\0' != *end) {
        return -1;
    } else if (errno == ERANGE) {
        return -1;
    }

    return 0;
}

int safe_strtoul(char *s, uint32_t *si)
{
    /* Convert string s -> 32 bit unsigned integer. */
    char *end;

    errno = 0;
    *si = strtoul(s, &end, 0);

    if (end == s) {
        return -1;
    } else if ('\0' != *end) {
        return -1;
    } else if (errno == ERANGE) {
        return -1;
    }

    return 0;
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
