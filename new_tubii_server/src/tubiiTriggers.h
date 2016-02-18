/*
 * triggers.h
 *
 *  Created on: Mar 20, 2015
 *      Author: Ian
 */

#ifndef TRIGGERS_H_
#define TRIGGERS_H_
#include <time.h>

int last_one=-1;
int last_ptr=0;
int err_flg=0;
int data_readout=0;

void *MappedCountBaseAddress;
void *MappedTrigBaseAddress;
void *MappedFifoBaseAddress;
void *MappedBurstBaseAddress;
void *MappedComboBaseAddress;
void *MappedPrescaleBaseAddress;

int trigMask, countMask, speakMask;

void burstTrig(char* bArg1, char* bArg2)
{
  double rate= (double) atof(bArg1);
  int mask= (int) atoi(bArg2);
  Log(NOTICE, "TUBii: Set rate for burst trigger: %d on mask %d",rate, mask);
  mWriteReg(MappedBurstBaseAddress, RegOffset2, rate);
  mWriteReg(MappedBurstBaseAddress, RegOffset3, mask);
  int j=0;
  while(j<9999){
	  printf("%x \t %x \t %x \t %x\n",mReadReg(MappedBurstBaseAddress,RegOffset0),mReadReg(MappedBurstBaseAddress,RegOffset1),mReadReg(MappedBurstBaseAddress,RegOffset2),mReadReg(MappedBurstBaseAddress,RegOffset3));
	  j++;
  }
}

void comboTrig(char* cArg1, char* cArg2)
{
  u32 enableMask= (u32) atoi(cArg1);
  u32 logicMask= (u32) atoi(cArg2);
  Log(NOTICE, "TUBii: Set mask for combo trigger: %d (%d)",logicMask,enableMask);
  mWriteReg(MappedComboBaseAddress, RegOffset2, enableMask);
  mWriteReg(MappedComboBaseAddress, RegOffset3, logicMask);
}

void prescaleTrig(char* pArg1, char* pArg2)
{
  double rate= (double) atof(pArg1);
  int mask= (int) atoi(pArg2);
  Log(NOTICE, "TUBii: Set rate for prescale trigger: %d on mask %i",rate,mask);
  mWriteReg(MappedPrescaleBaseAddress, RegOffset2, rate);
  mWriteReg(MappedPrescaleBaseAddress, RegOffset3, mask);

}

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

int counterMask(char* mask)
{
  int imask = atoi(mask);
  mWriteReg(MappedTrigBaseAddress, RegOffset1,imask);
}

int speakerMask(char* mask)
{
  int imask = atoi(mask);
  mWriteReg(MappedTrigBaseAddress, RegOffset2,imask);
}

int triggerMask(char* mask)
{
  int imask = atoi(mask);
  mWriteReg(MappedTrigBaseAddress, RegOffset3,imask);

  return 0;
}

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

#endif /* TRIGGERS_H_ */
