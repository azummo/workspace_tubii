#include "tubii_client.h"
#include "record_info.h"
#include "data.h"
#include "logging.h"
#include <stdlib.h>
#include <arpa/inet.h>
#include <errno.h>
#include "ae.h"
#include "server.h"
#include "db.h"

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
extern database *detector_db;
long long tubii_readout_id = AE_ERR;
int last_gtid=0;

static void save_db_callback(PGresult *res, PGconn *conn, void *data);
static void save_db_client_callback(PGresult *res, PGconn *conn, void *data);
static void load_db_callback(PGresult *res, PGconn *conn, void *data);
static void client_disconnect(void *data);
long long save_tubii_id = -1;

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
  //MappedSpeakLengthenBaseAddress= MemoryMapping(COUNTLENGTHEN_BASEADDR,COUNTLENGTHEN_HIGHADDR);

  // Trigger, Counter & Speaker Masks
  MappedTrigBaseAddress= MemoryMapping(TRIGGEROUT_BASEADDR,TRIGGEROUT_HIGHADDR);
  MappedGTIDBaseAddress= MemoryMapping(GTID_BASEADDR,GTID_HIGHADDR);
  MappedFifoBaseAddress= MemoryMapping(FIFOREADOUT_BASEADDR,FIFOREADOUT_HIGHADDR);
  MappedSpeakerScaleBaseAddress= MemoryMapping(SPEAKERSCALE_BASEADDR,SPEAKERSCALE_HIGHADDR);

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
  // Speaker mask to GT
  speakerMask(0x1000000);
  // Counter mask to GT
  counterMask(0x1000000);
  // Trigger mask to 0
  triggerMask(0,0);

  // Set Speaker pre-scale to 1
  //speakerScale(1);

  // Switch data readout on by default
  data_readout=1;

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
  Log(VERBOSE, "TUBii: Ping!");
  addReplyStatus(c, "+OK");
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
  save_tubii_state();
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
	  save_tubii_state();
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
  save_tubii_state();
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
  save_tubii_state();
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
  save_tubii_state();
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
  save_tubii_state();
  addReplyStatus(c, "+OK");
}

void SetCounterMask(client *c, int argc, sds *argv)
{
  uint32_t mask;
  safe_strtoul(argv[1],&mask);
  counterMask(mask);
  save_tubii_state();
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
  save_tubii_state();
  addReplyStatus(c, "+OK");
}

void SetSpeakerScale(client *c, int argc, sds *argv)
{
  uint32_t rate;
  safe_strtoul(argv[1],&rate);
  speakerScale(rate);
  save_tubii_state();
  addReplyStatus(c, "+OK");
}

void GetSpeakerMask(client *c, int argc, sds *argv)
{
  addReply(c, ":%u", getSpeakerMask());
}

void SetTriggerMask(client *c, int argc, sds *argv)
{
  uint32_t mask, mask_async;
  safe_strtoul(argv[1],&mask);
  safe_strtoul(argv[2],&mask_async);
  triggerMask(mask,mask_async);
  save_tubii_state();
  addReplyStatus(c, "+OK");
}

void GetSyncTriggerMask(client *c, int argc, sds *argv)
{
  addReply(c, ":%u", getSyncTriggerMask());
}

void GetAsyncTriggerMask(client *c, int argc, sds *argv)
{
  addReply(c, ":%u", getAsyncTriggerMask());
}

void SetBurstTrigger(client *c, int argc, sds *argv)
{
  float rate;
  uint32_t masterBit, slaveBit;
  safe_strtof(argv[1],&rate);
  safe_strtoul(argv[2],&masterBit);
  safe_strtoul(argv[3],&slaveBit);

  if(burstTrig(rate,masterBit,slaveBit) == 0){
	  save_tubii_state();
	  addReplyStatus(c, "+OK");
  }
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

  if(comboTrig(enableMask,logicMask) == 0){
	  save_tubii_state();
	  addReplyStatus(c, "+OK");
  }
  else addReplyError(c, tubii_err);
}

void SetPrescaleTrigger(client *c, int argc, sds *argv)
{
  float rate;
  uint32_t bit;
  safe_strtof(argv[1],&rate);
  safe_strtoul(argv[2],&bit);

  if(prescaleTrig(rate,bit) == 0){
	  save_tubii_state();
	  addReplyStatus(c, "+OK");
  }
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
  save_tubii_state();
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
void GetTrigger(client *c, int argc, sds *argv)
{
  addReply(c, ":%u", currentTrigger());
}

void GetGTID(client *c, int argc, sds *argv)
{
  addReply(c, ":%u", currentgtid());
}

void GetFifoTrigger(client *c, int argc, sds *argv)
{
    struct MegaRecord mega;
	struct GenericRecordHeader header;

    int size=0;
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

	/* convert to big endian */
    int i;
    for (i = 0; i < sizeof(trec)/4; i++) {
      ((uint32_t *)&trec)[i] = htonl(((uint32_t *)&trec)[i]);
    }
    mega.array[size]=trec;
    size = size+1;
    }
	}

	if(size>0){
    printf("Bundle!\n");
    printf("%i events!\n",size);

    header.RecordID = htonl(MEGA_RECORD);
    header.RecordLength = htonl(sizeof(u32)*(2*size));
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
	//Pulser(1,0.5,100,MappedHappyBaseAddress);

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

    int size=0;
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

			/* convert to big endian */
		    int i;
		    for (i = 0; i < sizeof(trec)/4; i++) {
		      ((uint32_t *)&trec)[i] = htonl(((uint32_t *)&trec)[i]);
		    }

			mega.array[size]=trec;
			//printf("%i \t %i\n",mega.size, last_gtid);
			size=size+1;
		}
	}

	if(size>0){
		//printf("Bundle! %i events!\n",mega.size);
		header.RecordID = htonl(MEGA_RECORD);
		header.RecordLength = htonl(sizeof(u32)*(2*size));
		header.RecordVersion = htonl(RECORD_VERSION);

		write_to_data_stream(&header, &mega);
    }

    return 1;
}

// Read the database configuration details from a config file
void load_new_config(client *c, int argc, sds *argv)
{
	auto_load_config(argv[1]);
    addReplyStatus(c, "+OK");
}

void auto_load_config(char* file)
{
	FILE *fp=fopen(file,"r");
	if(fp!=0){
    	char call[255];
    	while(fscanf(fp,"%s",call)!=EOF){
    		if(strcmp(call,"user")==0) fscanf(fp,"%s",dbconfig.user);
    		else if(strcmp(call,"pass")==0) fscanf(fp,"%s",dbconfig.password);
    		else if(strcmp(call,"dbname")==0) fscanf(fp,"%s",dbconfig.name);
    		else if(strcmp(call,"dbhost")==0) fscanf(fp,"%s",dbconfig.host);
    		else printf("TUBii: Unrecognised data type in config file, %s\n\n",call);
    	}
	}

	fclose(fp);
}

// There are two sets of functions for the database.
// - One writes to the database on command at the start of a run. This uses the functions:
//     save_TUBii_command, load_TUBii_command, save_db_client_callback, client disconnect
// - The second aims to give the current status of TUBii by writing to the row with key=0.
// This works from by calling the save_tubii_state function in any command which could change
// a database value. It then waits one second (in case multiple changes are being made) and
// the writes to the database. It uses the functions:
//     save_tubii_state, save_tubii, save_db_callback

void save_TUBii_command(client *c, int argc, sds *argv)
{
    /* Update the TUBii state. */
    char command[2048];// = sdsempty();

    // SELECT save_tubii
    // Writes to the next free row or (if settings match previous, returns that instead)
    sprintf(command, "SELECT save_tubii ("
                     "%u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u"
                     ");",
                     mReadReg((u32) MappedRegsBaseAddress, RegOffset10),
                     getSyncTriggerMask(), getSpeakerMask(), getCounterMask(),
                     mReadReg((u32) MappedRegsBaseAddress, RegOffset11),
                     mReadReg((u32) MappedRegsBaseAddress, RegOffset12),
                     mReadReg((u32) MappedRegsBaseAddress, RegOffset14),
                     mReadReg((u32) MappedRegsBaseAddress, RegOffset15),
                     mReadReg((u32) MappedRegsBaseAddress, RegOffset13),
                     mReadReg((u32) MappedComboBaseAddress, RegOffset2),
                     mReadReg((u32) MappedComboBaseAddress, RegOffset3),
                     counter_mode, clockStatus(),
                     mReadReg((u32) MappedPrescaleBaseAddress, RegOffset2),
                     mReadReg((u32) MappedPrescaleBaseAddress, RegOffset3),
                     mReadReg((u32) MappedBurstBaseAddress, RegOffset2),
                     mReadReg((u32) MappedBurstBaseAddress, RegOffset3)
                     );

    if (db_exec_async(detector_db, command, save_db_client_callback, c)) {
        addReplyError(c, "TUBii: database isn't connected");
        return;
    }

    blockClient(c, client_disconnect, c);
    return;

err:
    addReplyError(c, tubii_err);
    return;
}

void load_TUBii_command(client *c, int argc, sds *argv)
{
    /* Load TUBii hardware settings from the database. */
	uint32_t key;
    char command[3072];

    if (safe_strtoul(argv[1], &key)) {
        addReplyErrorFormat(c, "'%s' is not a valid uint32_t", argv[1]);
        return;
    }

    sprintf(command, "select * from TUBii where key = %i", key);

    load_db_args *args = (load_db_args *) malloc(sizeof(load_db_args));
    args->c = c;
    args->key = key;

    if (db_exec_async(detector_db, command, load_db_callback, args)) {
    	addReplyError(c, "database isn't connected");
    	free(args);
    	return;
    }

    blockClient(c, client_disconnect, c);

    return;
}

static void load_db_callback(PGresult *res, PGconn *conn, void *data)
{
    int i, j;
    char *name, *value_str;
    uint32_t value;
    int rows;
    load_db_args *args;

    args = (load_db_args *) data;

    client *c = args->c;

    if (c == NULL) {
        /* This should only happen if the client disconnects while a database
         * request was pending. */
        Log(WARNING, "tubii client got database response but is disconnected!");
        free(args);
        return;
    }

    if (res == NULL) {
        /* Request failed for some reason. */
        addReplyError(c, "request timed out or database disconnected");
        goto err;
    }

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        addReplyErrorFormat(c, "select command failed: %s",
                            PQerrorMessage(conn));
        goto err;
    }

    rows = PQntuples(res);
    if (rows != 1) {
        if (rows == 0) {
            addReplyErrorFormat(c, "no database row with key = %i", args->key);
        } else {
            addReplyError(c, "this should never happen. Call Tony");
        }
        goto err;
    }

    for (i = 0; i < PQnfields(res); i++) {
        name = PQfname(res, i);

        if (!strcmp(name, "key") || !strcmp(name, "timestamp")) continue;

        if (PQgetisnull(res, 0, i)) {
            addReplyErrorFormat(c, "column %s contains a NULL value", name);
            goto err;
        }

        value_str = PQgetvalue(res, 0, i);

        if (safe_strtoul(value_str, &value)) {
            addReplyErrorFormat(c, "unable to convert value '%s' for field %s",
                                value_str, name);
            goto err;
        }

        if (!strcmp(name, "control_reg")) {
        	ControlReg(value);
        } else if (!strcmp(name, "trigger_mask")) {
        	triggerMask(value,0);
        } else if (!strcmp(name, "speaker_mask")) {
            speakerMask(value);
        } else if (!strcmp(name, "counter_mask")) {
            counterMask(value);
        } else if (!strcmp(name, "caen_gain_reg")) {
        	CAENWords(value, mReadReg((u32) MappedRegsBaseAddress, RegOffset12));
        } else if (!strcmp(name, "caen_channel_reg")) {
        	CAENWords(mReadReg((u32) MappedRegsBaseAddress, RegOffset11), value);
        } else if (!strcmp(name, "lockout_reg")) {
        	GTDelays(value, mReadReg((u32) MappedRegsBaseAddress, RegOffset15));
        } else if (!strcmp(name, "dgt_reg")) {
        	GTDelays(mReadReg((u32) MappedRegsBaseAddress, RegOffset14), value);
        } else if (!strcmp(name, "dac_reg")) {
        	DACThresholds(value);
        } else if (!strcmp(name, "counter_mode")) {
        	counterMode(value);
        } else if (!strcmp(name, "clock_status")) {
        	// Do Nowt
        } else if (!strcmp(name, "combo_enable_mask")) {
        	mWriteReg((u32) MappedComboBaseAddress, RegOffset2, value);
        } else if (!strcmp(name, "combo_mask")) {
        	mWriteReg((u32) MappedComboBaseAddress, RegOffset3, value);
        } else if (!strcmp(name, "prescale_value")) {
        	mWriteReg((u32) MappedPrescaleBaseAddress, RegOffset2, value);
        } else if (!strcmp(name, "prescale_channel")) {
        	mWriteReg((u32) MappedPrescaleBaseAddress, RegOffset3, value);
        } else if (!strcmp(name, "burst_rate")) {
        	mWriteReg((u32) MappedBurstBaseAddress, RegOffset2, value);
        } else if (!strcmp(name, "burst_channel")) {
        	mWriteReg((u32) MappedBurstBaseAddress, RegOffset3, value);
        } else {
            addReplyErrorFormat(c, "got unknown field '%s'", name);
            goto err;
        }
    }

    save_tubii_state();

    addReplyStatus(c, "OK");

    unblockClient(c);
    free(args);

    return;

err:
    unblockClient(c);
    free(args);
    return;
}

static void save_db_client_callback(PGresult *res, PGconn *conn, void *data)
{
    /* Get result of save command. */
    uint32_t key;

    client *c = (client *) data;

    if (c == NULL) {
        /* This should only happen if the client disconnects while a database
         * request was pending. */
        Log(WARNING, "TUBii: client got database response but is disconnected!");
        return;
    }

    if (res == NULL) {
        /* Request failed for some reason. */
        addReplyError(c, "TUBii: request timed out or database disconnected");
        goto err;
    }

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        addReplyErrorFormat(c, "TUBii: insert command failed: %s",
                            PQerrorMessage(conn));
        goto err;
    }

    if (PQnfields(res) != 1) {
        addReplyError(c, "TUBii: failed to get key from insert");
        goto err;
    }

    if (safe_strtoul(PQgetvalue(res, 0, 0), &key)) {
        addReplyErrorFormat(c, "TUBii: couldn't convert key from '%s' -> int",
                            PQgetvalue(res, 0, 0));
        goto err;
    }

    addReplyLongLong(c, key);
    unblockClient(c);
    return;

err:
    unblockClient(c);
    return;
}

static void client_disconnect(void *data)
{
    /* Called when a client disconnects while they are blocked waiting
     * for a response from the database. Need to clear all requests sent by
     * this client. */
    client *c = (client *) data;

    clear_db_requests_from_client(detector_db, c);
}

static int save_tubii(aeEventLoop *el, long long id, void *data)
{
    /* Saves the TUBii hardware settings to the detector database. */
    char command[3072];

    // Inserts the current state into row 0
    sprintf(command, "INSERT INTO TUBii ("
                     "key,control_reg, trigger_mask, speaker_mask, counter_mask,"
                     "caen_gain_reg, caen_channel_reg, lockout_reg, dgt_reg, dac_reg,"
                     "combo_enable_mask, combo_mask, counter_mode, clock_status,"
                     "prescale_value, prescale_channel, burst_rate, burst_channel"
                     ") "
                     "VALUES (0, "
                     "%u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u)"
                     " ON CONFLICT (key) DO UPDATE SET "
                     "control_reg = EXCLUDED.control_reg,"
                     "trigger_mask = EXCLUDED.trigger_mask,"
                     "speaker_mask = EXCLUDED.speaker_mask,"
                     "counter_mask = EXCLUDED.counter_mask,"
                     "caen_gain_reg = EXCLUDED.caen_gain_reg,"
                     "caen_channel_reg = EXCLUDED.caen_channel_reg,"
                     "lockout_reg = EXCLUDED.lockout_reg,"
                     "dgt_reg = EXCLUDED.dgt_reg,"
                     "dac_reg = EXCLUDED.dac_reg,"
                     "combo_enable_mask = EXCLUDED.combo_enable_mask,"
                     "combo_mask = EXCLUDED.combo_mask,"
                     "counter_mode = EXCLUDED.counter_mode,"
                     "clock_status = EXCLUDED.clock_status,"
                     "prescale_value = EXCLUDED.prescale_value,"
                     "prescale_channel = EXCLUDED.prescale_channel,"
                     "burst_rate = EXCLUDED.burst_rate,"
                     "burst_channel = EXCLUDED.burst_channel;",
                     mReadReg((u32) MappedRegsBaseAddress, RegOffset10),
                     getSyncTriggerMask(), getSpeakerMask(), getCounterMask(),
                     mReadReg((u32) MappedRegsBaseAddress, RegOffset11),
                     mReadReg((u32) MappedRegsBaseAddress, RegOffset12),
                     mReadReg((u32) MappedRegsBaseAddress, RegOffset14),
                     mReadReg((u32) MappedRegsBaseAddress, RegOffset15),
                     mReadReg((u32) MappedRegsBaseAddress, RegOffset13),
                     mReadReg((u32) MappedComboBaseAddress, RegOffset2),
                     mReadReg((u32) MappedComboBaseAddress, RegOffset3),
                     counter_mode, clockStatus(),
                     mReadReg((u32) MappedPrescaleBaseAddress, RegOffset2),
                     mReadReg((u32) MappedPrescaleBaseAddress, RegOffset3),
                     mReadReg((u32) MappedBurstBaseAddress, RegOffset2),
                     mReadReg((u32) MappedBurstBaseAddress, RegOffset3)
                     );

    if (db_exec_async(detector_db, command, save_db_callback, NULL)) {
        Log(WARNING, "database isn't connected to save tubii state");
        save_tubii_id = -1;
        return AE_NOMORE;
    }

    save_tubii_id = -1;
    return AE_NOMORE;

err:
    Log(WARNING, "failed to save tubii state");
    save_tubii_id = -1;
    return AE_NOMORE;
}

static void save_db_callback(PGresult *res, PGconn *conn, void *data)
{
    /* Get result of save command. */
    uint32_t key;

    if (res == NULL) {
        /* Request failed for some reason. */
        Log(WARNING, "failed to save tubii state: request timed out or database disconnected");
        goto err;
    }

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        Log(WARNING, "failed to save tubii state: insert command failed: %s",
            PQerrorMessage(conn));
        goto err;
    }

    if (PQnfields(res) != 1) {
        Log(WARNING, "failed to save tubii state: failed to get key from insert");
        goto err;
    }

    if (safe_strtoul(PQgetvalue(res, 0, 0), &key)) {
        Log(WARNING, "failed to save tubii state: couldn't convert key from '%s' -> int",
            PQgetvalue(res, 0, 0));
        goto err;
    }

    return;

err:
    return;
}

void save_tubii_state()
{
    /* Set up an event to save the current TUBii state to the database in
     * one second if one isn't already scheduled. */

    if (save_tubii_id == -1) {
        if ((save_tubii_id = aeCreateTimeEvent(el, 1000, save_tubii, NULL, NULL)) == AE_ERR) {
            Log(WARNING, "failed to set up timed event to save tubii state to database");
            save_tubii_id = -1;
        }
    }
}

// Internal XADC functions -- NOT CURRENTLY WORKING. INTENDED TO POLL TEMPERATURE OF TUBII.
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

    printf("Self-test\n");
    //self test
    Status_ADC = XAdcPs_SelfTest(XADCInstPtr);
 	if (Status_ADC != XST_SUCCESS) {
 	  printf("Failed Self-test!\n");
 		addReplyStatus(c, "+NOT OK");
 	}

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

// String to float and int conversions
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
