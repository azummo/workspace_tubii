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
	  Log(WARNING, "TUBii: Choose a burst trigger bit between 0 and 15.");
	  sprintf(tubii_err, "TUBii: Choose a burst trigger bit between 0 and 15.");
	  return -1;
  }

  int mask = pow(2,bit);
  Log(NOTICE, "TUBii: Set rate for burst trigger: %lf on bit %i",rate,bit);
  mWriteReg(MappedBurstBaseAddress, RegOffset2, rate);
  mWriteReg(MappedBurstBaseAddress, RegOffset3, mask);
  return 0;
}

int buttonTrig()
{
  Log(NOTICE, "TUBii: Fire the button trigger.");
  mWriteReg(MappedButtonBaseAddress, RegOffset0, 1);
  return 0;
}

int comboTrig(u32 enableMask, u32 logicMask)
{
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

int prescaleTrig(float rate, int bit)
{
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
int counterLatch(int latch)
{
  mWriteReg(MappedCountBaseAddress, RegOffset0, latch);
  return 0;
}

int counterReset(int reset)
{
  mWriteReg(MappedCountBaseAddress, RegOffset1, reset);
  return 0;
}

int counterMode(int mode)
{
  counter_mode = mode;
  if(counter_mode == 0) counterLatch("0");
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
  mWriteReg(MappedCountLengthenBaseAddress, RegOffset1,200); // Fix the pulse length
  mWriteReg(MappedCountLengthenBaseAddress, RegOffset2,400); // And deadtime
  mWriteReg(MappedTrigBaseAddress, RegOffset1,mask);

  return mask;
}

int getCounterMask()
{
  return mReadReg(MappedTrigBaseAddress, RegOffset1);
}

int speakerMask(u32 mask)
{
  mWriteReg(MappedTrigBaseAddress, RegOffset2,mask);
  return mask;
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

u32 getTriggerMask()
{
  return mReadReg(MappedTrigBaseAddress,RegOffset3);
}

void softGT()
{
  mWriteReg(MappedTrigBaseAddress, RegOffset0,2147483648);
  usleep(100);
  mWriteReg(MappedTrigBaseAddress, RegOffset0, 0);
}

void resetGTID()
{
  mWriteReg(MappedTrigBaseAddress, RegOffset0,1073741824);
  //mWriteReg(MappedTrigBaseAddress, RegOffset0,3221225472);
  usleep(1);
  mWriteReg(MappedTrigBaseAddress, RegOffset0, 0);
}

/////// Data Readout
u32 currentTrigger()
{
  u32 current_trig= mReadReg(MappedTrigBaseAddress, RegOffset0) && 0xFFFFFF;
  u32 trig_mask= mReadReg(MappedTrigBaseAddress, RegOffset3);
  u32 gtid= mReadReg(MappedTrigBaseAddress, RegOffset4);
  printf("Current trig is: %u with mask %u and gtid %u\n", current_trig, trig_mask, gtid);
  return current_trig;
}

void fifoTrigger(struct TubiiRecord* record)
{
  int error;
  mWriteReg(MappedFifoBaseAddress, RegOffset0,1);
  record->TrigWord = mReadReg(MappedFifoBaseAddress, RegOffset1) & 0xFFFFFF;
  record->GTID = mReadReg(MappedFifoBaseAddress, RegOffset2) & 0xFFFFFF;
  error= mReadReg(MappedFifoBaseAddress, RegOffset3);
  mWriteReg(MappedFifoBaseAddress, RegOffset0,0);

  if(error>1 && err_flg==0){
	  //printf("Error is: %i \n", error);
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

void enableFIFO()
{
  mWriteReg(MappedFifoBaseAddress, RegOffset0,12);
}

int TrigWordDelay(u32 delay)
{
  if(delay<0){
	Log(WARNING, "TUBii: delay length is outside acceptable range.");
	sprintf(tubii_err, "Tubii: delay length is outside acceptable range.");
	return -1;
  }

  mWriteReg(MappedTrigWordDelayBaseAddress, RegOffset0, delay);
  return 0;
}

#endif /* TRIGGERS_H_ */
