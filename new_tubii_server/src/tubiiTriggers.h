/*
 * triggers.h
 *
 *  Created on: Mar 20, 2015
 *      Author: Ian
 */

#ifndef TRIGGERS_H_
#define TRIGGERS_H_
#include <time.h>
#include <math.h>

int last_one=-1;
int last_ptr=0;
int err_flg=0;
int data_readout=0;
int cmode=0;

void *MappedCountBaseAddress;
void *MappedTrigBaseAddress;
void *MappedFifoBaseAddress;
void *MappedBurstBaseAddress;
void *MappedComboBaseAddress;
void *MappedPrescaleBaseAddress;
void *MappedCountLengthenBaseAddress;
void* MappedTrigWordDelayBaseAddress;
void* MappedGTIDBaseAddress;

/////// Internal Triggers
int burstTrig(char* bArg1, char* bArg2)
{
  double rate= (double) atof(bArg1);
  int bit= (int) atoi(bArg2);

  if(bit<0 || bit>15){
	  Log(WARNING, "TUBii: Choose a burst trigger bit between 0 and 15.");
	  sprintf(tubii_err, "TUBii: Choose a burst trigger bit between 0 and 15.");
	  return -1;
  }

  int mask = pow(2,bit);
  Log(NOTICE, "TUBii: Set rate for burst trigger: %lf on bit %i",rate,bit);
  mWriteReg(MappedBurstBaseAddress, RegOffset2, rate);
  mWriteReg(MappedBurstBaseAddress, RegOffset3, mask);
  int i=0;
  while(i<1000){
    printf("%u \t %u \t %u \t %u \n",mReadReg(MappedBurstBaseAddress,RegOffset0),mReadReg(MappedBurstBaseAddress,RegOffset1),mReadReg(MappedBurstBaseAddress,RegOffset2),mReadReg(MappedBurstBaseAddress,RegOffset3));
	i++;
  }
  return 0;
}

int comboTrig(char* cArg1, char* cArg2)
{
  u32 enableMask= (u32) atoi(cArg1);
  u32 logicMask= (u32) atoi(cArg2);

  if(logicMask<0 || logicMask>65535 || enableMask<0 || enableMask>65535){
	  Log(WARNING, "TUBii: Choose a combo trigger mask between 0 and 65535.");
	  sprintf(tubii_err, "TUBii: Choose a combo trigger mask between 0 and 65535.");
	  return -1;
  }

  Log(NOTICE, "TUBii: Set mask for combo trigger: %d (%d)",logicMask,enableMask);
  mWriteReg(MappedComboBaseAddress, RegOffset2, enableMask);
  mWriteReg(MappedComboBaseAddress, RegOffset3, logicMask);
  return 0;
}

int prescaleTrig(char* pArg1, char* pArg2)
{
  double rate= (double) atof(pArg1);
  int bit= (int) atoi(pArg2);

  if(bit<0 || bit>15){
	  Log(WARNING, "TUBii: Choose a prescale trigger bit between 0 and 15.");
	  sprintf(tubii_err, "TUBii: Choose a prescale trigger bit between 0 and 15.");
	  return -1;
  }

  int mask = pow(2,bit);
  Log(NOTICE, "TUBii: Set rate for prescale trigger: %lf on bit %i",rate,bit);
  mWriteReg(MappedPrescaleBaseAddress, RegOffset2, rate);
  mWriteReg(MappedPrescaleBaseAddress, RegOffset3, mask);
  return 0;
}

/////// Counters and Speakers
int counterLatch(char* length)
{
	int imask = atoi(length);
	Log(NOTICE, "TUBii: Set counter latch: %i",imask);
	mWriteReg(MappedCountBaseAddress, RegOffset0, imask);

	return 0;
}

int counterReset(char* length)
{
	int imask = atoi(length);
	Log(NOTICE, "TUBii: Set counter reset: %i",imask);
	mWriteReg(MappedCountBaseAddress, RegOffset1, imask);

	return 0;
}

int counterMode(char* length)
{
	cmode = atoi(length);
	return cmode;
}

void GetRate()
{
	// Toggle Latch
	counterLatch("0");
	usleep(1);
	counterLatch("1");
	usleep(1);
	counterReset("0");
	usleep(1);
	counterReset("1");
}

int counterMask(char* mask)
{
  uint32_t imask;
  safe_strtoul(mask,&imask);
  mWriteReg(MappedCountLengthenBaseAddress, RegOffset1,200); // Fix the pulse length
  mWriteReg(MappedCountLengthenBaseAddress, RegOffset2,400); // And deadtime
  mWriteReg(MappedTrigBaseAddress, RegOffset1,imask);

  return imask;
}

int getCounterMask()
{
  return mReadReg(MappedTrigBaseAddress, RegOffset1);
}

int speakerMask(char* mask)
{
  uint32_t imask;
  safe_strtoul(mask,&imask);
  mWriteReg(MappedTrigBaseAddress, RegOffset2,imask);

  return imask;
}

int getSpeakerMask()
{
  return mReadReg(MappedTrigBaseAddress, RegOffset2);
}

u32 triggerMask(char* mask)
{
  uint32_t imask;
  safe_strtoul(mask,&imask);
  mWriteReg(MappedTrigBaseAddress, RegOffset3,imask);

  return imask;
}

u32 resetGTID()
{
  mWriteReg(MappedGTIDBaseAddress, RegOffset0,0);

  return 0;
}

u32 getTriggerMask()
{
  return mReadReg(MappedTrigBaseAddress,RegOffset3);
}

/////// Data Readout
struct TubiiRecord triggerReport()
{
  int current_trig= mReadReg(MappedTrigBaseAddress, RegOffset0);
  int trig_mask= mReadReg(MappedTrigBaseAddress, RegOffset3);

  mWriteReg(MappedFifoBaseAddress, RegOffset3,4);
  int trig= mReadReg(MappedFifoBaseAddress, RegOffset0) & 0xFFFFFF;
  int gtid= mReadReg(MappedFifoBaseAddress, RegOffset1) & 0xFFFFFF;
  int error= mReadReg(MappedFifoBaseAddress, RegOffset2);
  int ptrs= mReadReg(MappedFifoBaseAddress, RegOffset3);
  int readptr = (ptrs & 0xFF000000) >> 24;
  int writeptr = (ptrs & 0xFF0000) >> 16;
  error = error & 3;

  /*if(error == 2){
	 //printf("FIFO is up to date\n");
	 struct TubiiRecord EmptyRecord;
	 EmptyRecord.GTID = -1;
	 EmptyRecord.TrigWord = -1;
	 return EmptyRecord;
  }*/

  //printf("Current trig is: %i with mask %i\n", current_trig, trig_mask);
  //printf("Fifo trig is: %i with GTID: %i \n", trig, gtid);
  //printf("Error is: %i --- RPtr: %i WPtr: %i\n", error, readptr, writeptr);

  if(last_one != gtid-1 && last_one!=gtid){
	  printf("Misreport: %i --> %i\n", last_one, gtid);
	  printf("Misptr: %i --> %i\n", last_ptr, readptr);
  }
  last_one=gtid;
  last_ptr=readptr;

  struct TubiiRecord record;
  record.GTID = gtid;
  record.TrigWord = trig;

  return record;
}

void currentTrigger()
{
  //struct TubiiRecord record;
  int current_trig= mReadReg(MappedTrigBaseAddress, RegOffset0);
  int trig_mask= mReadReg(MappedTrigBaseAddress, RegOffset3);
  printf("Current trig is: %i with mask %i\n", current_trig, trig_mask);
}

void fifoTrigger(struct TubiiRecord* record)
{
  mWriteReg(MappedFifoBaseAddress, RegOffset0,1);
  record->TrigWord = mReadReg(MappedFifoBaseAddress, RegOffset1) & 0xFFFFFF;
  record->GTID = mReadReg(MappedFifoBaseAddress, RegOffset2) & 0xFFFFFF;
  int error= mReadReg(MappedFifoBaseAddress, RegOffset3);
  mWriteReg(MappedFifoBaseAddress, RegOffset0,0);

  if(error>1 && err_flg==0){
	  printf("Error is: %i \n", error);
	  err_flg=1;
  }
  else if(error<2) err_flg=0;
}

void resetFIFO()
{
  mWriteReg(MappedFifoBaseAddress, RegOffset0,2);
  usleep(1000);
  mWriteReg(MappedFifoBaseAddress, RegOffset0,0);
}

int TrigWordDelay(char* dArg)
{
  double ns = 0.1;
  double length= (double) atof(dArg);
  u32 delay = length*ns;
  if(delay<0){
	Log(WARNING, "TUBii: delay length is outside acceptable range.");
	sprintf(tubii_err, "Tubii: delay length is outside acceptable range.");
	return -1;
  }

  Log(NOTICE, "TUBii: delay length is %s ns.", dArg);

  // Set Delay
  mWriteReg(MappedTrigWordDelayBaseAddress, RegOffset0, delay);

  return 0;
}

#endif /* TRIGGERS_H_ */
