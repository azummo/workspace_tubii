/*
 * triggers.h
 *
 *  Created on: Mar 20, 2015
 *      Author: Ian
 */

#ifndef TRIGGERS_H_
#define TRIGGERS_H_

void burstTrig(char* argument)
{
  u32 BaseAddress= BURSTTRIG_BASEADDR;
  u32 HighAddress= BURSTTRIG_HIGHADDR;
  void *MappedBaseAddress= MemoryMapping(BaseAddress,HighAddress);

  double rate= (double) atof(argument);
  printf("Set rate for burst trigger: %d\n",rate);
  mWriteReg(MappedBaseAddress, RegOffset3, rate);

  while(1){
  printf("%d \t",mReadReg(MappedBaseAddress, RegOffset0));
  printf("%d \t",mReadReg(MappedBaseAddress, RegOffset1));
  printf("%d \t",mReadReg(MappedBaseAddress, RegOffset2));
  printf("%d \n",mReadReg(MappedBaseAddress, RegOffset3));
  }

  munmap((void*)MappedBaseAddress, (HighAddress-BaseAddress));
}

void comboTrig(char* argument)
{
  u32 BaseAddress= COMBOTRIG_BASEADDR;
  u32 HighAddress= COMBOTRIG_HIGHADDR;
  void *MappedBaseAddress= MemoryMapping(BaseAddress,HighAddress);

  u32 mask= (u32) atoi(argument);
  printf("Set mask for combo trigger: %d\n",mask);
  mWriteReg(MappedBaseAddress, RegOffset3, mask);

  munmap((void*)MappedBaseAddress, (HighAddress-BaseAddress));
}

void prescaleTrig(char* argument)
{
  u32 BaseAddress= PRESCALETRIG_BASEADDR;
  u32 HighAddress= PRESCALETRIG_HIGHADDR;
  void *MappedBaseAddress= MemoryMapping(BaseAddress,HighAddress);

  double rate= (double) atof(argument);
  printf("Set rate for Prescale trigger: %d\n",rate);
  mWriteReg(MappedBaseAddress, RegOffset1, rate);

  while(1){
  printf("%d \t",mReadReg(MappedBaseAddress, RegOffset0));
  printf("%d \t",mReadReg(MappedBaseAddress, RegOffset1));
  printf("%d \t",mReadReg(MappedBaseAddress, RegOffset2));
  printf("%d \n",mReadReg(MappedBaseAddress, RegOffset3));
  }

  munmap((void*)MappedBaseAddress, (HighAddress-BaseAddress));
}

void triggerEnable(int mask)
{
  u32 BaseAddress= TRIGGEROUT_BASEADDR;
  u32 HighAddress= TRIGGEROUT_HIGHADDR;
  void *MappedBaseAddress= MemoryMapping(BaseAddress,HighAddress);

  mWriteReg(MappedBaseAddress, RegOffset3,mask);

  munmap((void*)MappedBaseAddress, (HighAddress-BaseAddress));
}

void GTpoll(int go, int base)
{
  u32 BaseAddress= GTPOLL_BASEADDR;
  u32 HighAddress= GTPOLL_HIGHADDR;
  void *MappedBaseAddress= MemoryMapping(BaseAddress,HighAddress);

  mWriteReg(MappedBaseAddress, RegOffset0,base);

  int prevtrig=-1, currtrig=-1;
  while(1){
	currtrig = mReadReg(MappedBaseAddress, RegOffset0);
	if(prevtrig!=currtrig) printf("%i \n", currtrig);
	prevtrig = currtrig;
  }

  munmap((void*)MappedBaseAddress, (HighAddress-BaseAddress));
}

int triggerMask(char* mask)
{
  u32 BaseAddress= TRIGGEROUT_BASEADDR;
  u32 HighAddress= TRIGGEROUT_HIGHADDR;
  void *MappedBaseAddress= MemoryMapping(BaseAddress,HighAddress);
  int imask = atoi(mask);

  mWriteReg(MappedBaseAddress, RegOffset3,imask);

  munmap((void*)MappedBaseAddress, (HighAddress-BaseAddress));
  return 0;
}

int triggerReport()
{
  u32 BaseAddress= FIFO_BASEADDR;
  u32 HighAddress= FIFO_HIGHADDR;
  void *MappedBaseAddress= MemoryMapping(BaseAddress,HighAddress);

  u32 BaseAddressX= TRIGGEROUT_BASEADDR;
  u32 HighAddressX= TRIGGEROUT_HIGHADDR;
  void *MappedBaseAddressX= MemoryMapping(BaseAddressX,HighAddressX);

  char trigBuffer[BUFFER_SIZE];
  int trig=0, trig0, error=0, horse=0, horse2=0, red=0, writ=0;

  mWriteReg(MappedBaseAddress, RegOffset2,1);
  //mWriteReg(MappedBaseAddress, RegOffset2,2);
  trig0= mReadReg(MappedBaseAddress, RegOffset0) & 0xFFFFFF;
  trig= mReadReg(MappedBaseAddress, RegOffset1) & 0xFFFFFF;
  error= mReadReg(MappedBaseAddress, RegOffset3);
  red = (error & 0xFF000000) >> 24;
  writ = (error & 0xFF0000) >> 16;
  error = error & 3;
  horse= mReadReg(MappedBaseAddressX, RegOffset0);
  horse2= mReadReg(MappedBaseAddressX, RegOffset3);
  printf("Source is: %i with mask %i\n", horse, horse2);
  printf("CurrentTrig is: %i \n", trig0);
  printf("Trigbuffer is: %i \n", trig);
  printf("Error is: %i \n", error);
  printf("Read is: %i and Write is: %i\n", red, writ);
  mWriteReg(MappedBaseAddress, RegOffset2,0);

  munmap((void*)MappedBaseAddress, (HighAddress-BaseAddress));
  munmap((void*)MappedBaseAddressX, (HighAddressX-BaseAddressX));
  return trig;
}

#endif /* TRIGGERS_H_ */
