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
int counter_mode=0;

void *MappedCountBaseAddress;
void *MappedTrigBaseAddress;
void *MappedFifoBaseAddress;
void *MappedBurstBaseAddress;
void *MappedButtonBaseAddress;
void *MappedComboBaseAddress;
void *MappedPrescaleBaseAddress;
void *MappedCountLengthenBaseAddress;
void* MappedTrigWordDelayBaseAddress;
void* MappedGTIDBaseAddress;

/////// Internal Triggers
int burstTrig(float rate, int bit)
{
  if(bit<0 || bit>15){
	  Log(VERBOSE, "TUBii: Choose a burst trigger bit between 0 and 15.");
	  sprintf(tubii_err, "TUBii: Choose a burst trigger bit between 0 and 15.");
	  return -1;
  }

  int mask = pow(2,bit);
  Log(VERBOSE, "TUBii: Set rate for burst trigger: %lf on bit %i",rate,bit);
  mWriteReg((u32) MappedBurstBaseAddress, RegOffset2, rate);
  mWriteReg((u32) MappedBurstBaseAddress, RegOffset3, mask);
  return 0;
}

int buttonTrig()
{
  Log(VERBOSE, "TUBii: Fire the button trigger.");
  mWriteReg((u32) MappedButtonBaseAddress, RegOffset0, 1);
  return 0;
}

int comboTrig(u32 enableMask, u32 logicMask)
{
  if(logicMask<0 || logicMask>65535 || enableMask<0 || enableMask>65535){
	  Log(VERBOSE, "TUBii: Choose a combo trigger mask between 0 and 65535.");
	  sprintf(tubii_err, "TUBii: Choose a combo trigger mask between 0 and 65535.");
	  return -1;
  }

  Log(VERBOSE, "TUBii: Set mask for combo trigger: %d (%d)",logicMask,enableMask);
  mWriteReg((u32) MappedComboBaseAddress, RegOffset2, enableMask);
  mWriteReg((u32) MappedComboBaseAddress, RegOffset3, logicMask);
  return 0;
}

int prescaleTrig(float rate, int bit)
{
  if(bit<0 || bit>15){
	  Log(VERBOSE, "TUBii: Choose a prescale trigger bit between 0 and 15.");
	  sprintf(tubii_err, "TUBii: Choose a prescale trigger bit between 0 and 15.");
	  return -1;
  }

  int mask = pow(2,bit);
  Log(VERBOSE, "TUBii: Set rate for prescale trigger: %lf on bit %i",rate,bit);
  mWriteReg((u32) MappedPrescaleBaseAddress, RegOffset2, rate);
  mWriteReg((u32) MappedPrescaleBaseAddress, RegOffset3, mask);
  return 0;
}

/////// Counters and Speakers
int counterLatch(int latch)
{
  mWriteReg((u32) MappedCountBaseAddress, RegOffset0, latch);
  return 0;
}

int counterReset(int reset)
{
  mWriteReg((u32) MappedCountBaseAddress, RegOffset1, reset);
  return 0;
}

int counterMode(int mode)
{
  counter_mode = mode;
  if(counter_mode == 0) counterLatch(0);
  return counter_mode;
}

void GetRate()
{
  // Toggle Latch
  counterLatch(0);
  usleep(1);
  counterLatch(1);
  usleep(1);
  counterReset(0);
  usleep(1);
  counterReset(1);
}

int counterMask(u32 mask)
{
  mWriteReg((u32) MappedCountLengthenBaseAddress, RegOffset1,200); // Fix the pulse length
  mWriteReg((u32) MappedCountLengthenBaseAddress, RegOffset2,400); // And deadtime
  mWriteReg((u32) MappedTrigBaseAddress, RegOffset1,mask);

  return mask;
}

int getCounterMask()
{
  return mReadReg((u32) MappedTrigBaseAddress, RegOffset1);
}

int speakerMask(u32 mask)
{
  mWriteReg((u32) MappedTrigBaseAddress, RegOffset2,mask);
  return mask;
}

int getSpeakerMask()
{
  return mReadReg((u32) MappedTrigBaseAddress, RegOffset2);
}

u32 triggerMask(char* mask)
{
  uint32_t imask;
  safe_strtoul(mask,&imask);
  mWriteReg((u32) MappedTrigBaseAddress, RegOffset3,imask);

  return imask;
}

u32 getTriggerMask()
{
  return mReadReg((u32) MappedTrigBaseAddress,RegOffset3);
}

void softGT()
{
  mWriteReg((u32) MappedTrigBaseAddress, RegOffset6,1);
  usleep(100);
  mWriteReg((u32) MappedTrigBaseAddress, RegOffset6, 0);
}

void resetGTID()
{
  mWriteReg((u32) MappedTrigBaseAddress, RegOffset7,1);
  usleep(1000);
  mWriteReg((u32) MappedTrigBaseAddress, RegOffset7, 0);
}

/////// Data Readout
u32 currentTrigger()
{
  u32 current_trig= mReadReg((u32) MappedTrigBaseAddress, RegOffset0) && 0xFFFFFF;
  u32 trig_mask= mReadReg((u32) MappedTrigBaseAddress, RegOffset3);
  u32 gtid= mReadReg((u32) MappedTrigBaseAddress, RegOffset4);
  u32 sync= mReadReg((u32) MappedTrigBaseAddress, RegOffset5);
  Log(NOTICE, "Current trig is: %lu with mask %lu and gtid %lu and sync %lu\n", current_trig, trig_mask, gtid, sync);
  return current_trig;
}

u32 currentgtid()
{
  u32 gtid= mReadReg((u32) MappedTrigBaseAddress, RegOffset4);
  return gtid;
}

void fifoTrigger(struct TubiiRecord* record)
{
  int error;
  mWriteReg((u32) MappedFifoBaseAddress, RegOffset0,1);
  record->TrigWord = mReadReg((u32) MappedFifoBaseAddress, RegOffset1) & 0xFFFFFF;
  record->GTID = mReadReg((u32) MappedFifoBaseAddress, RegOffset2) & 0xFFFFFF;
  error= mReadReg((u32) MappedFifoBaseAddress, RegOffset3);
  mWriteReg((u32) MappedFifoBaseAddress, RegOffset0,0);

  if(error>1 && err_flg==0){
	  //printf("Error is: %i \n", error);
	  err_flg=1;
  }
  else if(error<2) err_flg=0;
}

u32 fifoStatus()
{
  u32 error= mReadReg((u32) MappedFifoBaseAddress, RegOffset3);
  return error;
}

void resetFIFO()
{
  mWriteReg((u32) MappedFifoBaseAddress, RegOffset0,2);
  usleep(1000);
  mWriteReg((u32) MappedFifoBaseAddress, RegOffset0,0);
}

void enableFIFO()
{
  mWriteReg((u32) MappedFifoBaseAddress, RegOffset0,12);
}

int TrigWordDelay(u32 delay)
{
  if(delay<0){
	Log(VERBOSE, "TUBii: delay length is outside acceptable range.");
	sprintf(tubii_err, "Tubii: delay length is outside acceptable range.");
	return -1;
  }

  mWriteReg((u32) MappedTrigWordDelayBaseAddress, RegOffset0, delay);
  return 0;
}

#endif /* TRIGGERS_H_ */
