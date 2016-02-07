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

void ComboTrig(char* argument)
{
  u32 BaseAddress= COMBOTRIG_BASEADDR;
  u32 HighAddress= COMBOTRIG_HIGHADDR;
  void *MappedBaseAddress= MemoryMapping(BaseAddress,HighAddress);

  u32 mask= (u32) atoi(argument);
  printf("Set mask for combo trigger: %d\n",mask);
  mWriteReg(MappedBaseAddress, RegOffset3, mask);

  munmap((void*)MappedBaseAddress, (HighAddress-BaseAddress));
}

void Nothing(char* argument)
{
  u32 BaseAddress= NOTHING_BASEADDR;
  u32 HighAddress= NOTHING_HIGHADDR;
  void *MappedBaseAddress= MemoryMapping(BaseAddress,HighAddress);
  printf("mapped\n");

  u32 mask= (u32) atoi(argument);
  while(1){
  printf("Back from nothing: %d\n",mReadReg(MappedBaseAddress, RegOffset3));
  sleep(1);
  }

  munmap((void*)MappedBaseAddress, (HighAddress-BaseAddress));
}

void FanInTrig(char* argument)
{
  u32 BaseAddress= TRIGFANIN_BASEADDR;
  u32 HighAddress= TRIGFANIN_HIGHADDR;
  void *MappedBaseAddress= MemoryMapping(BaseAddress,HighAddress);

  u32 mask= (u32) atoi(argument);
  printf("Set mask for trigger fan in: %d\n",mask);
  mWriteReg(MappedBaseAddress, RegOffset2, mask);

  while(1){
  printf("%d \t",mReadReg(MappedBaseAddress, RegOffset0));
  printf("%d \t",mReadReg(MappedBaseAddress, RegOffset1));
  printf("%d \t",mReadReg(MappedBaseAddress, RegOffset2));
  printf("%d \n",mReadReg(MappedBaseAddress, RegOffset3));
  }

  munmap((void*)MappedBaseAddress, (HighAddress-BaseAddress));
}

void PrescaleTrig(char* argument)
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

void triggerReport(void* arg)
{
  u32 BaseAddress= FIFO_BASEADDR;
  u32 HighAddress= FIFO_HIGHADDR;
  void *MappedBaseAddress= MemoryMapping(BaseAddress,HighAddress);

  u32 BaseAddressX= TRIGGEROUT_BASEADDR;
  u32 HighAddressX= TRIGGEROUT_HIGHADDR;
  void *MappedBaseAddressX= MemoryMapping(BaseAddressX,HighAddressX);

  int hSocket = (int) arg;
  printf("Start trigger report \n");

  char trigBuffer[BUFFER_SIZE];
  int trig=0, trig0, inty=0, error=0, horse=0;
  mWriteReg(MappedBaseAddress, RegOffset3,255);
  int prevtrig=-1;
  while(1){
	mWriteReg(MappedBaseAddress, RegOffset2,1);
	//mWriteReg(MappedBaseAddress, RegOffset2,2);
	trig0= mReadReg(MappedBaseAddress, RegOffset0);
	trig= mReadReg(MappedBaseAddress, RegOffset1);
	error= mReadReg(MappedBaseAddress, RegOffset3) & 3;
	horse= mReadReg(MappedBaseAddressX, RegOffset0);
	sprintf(trigBuffer, "%d", horse);//trig);
	write(hSocket,trigBuffer,strlen(trigBuffer)+1);
	if(trig==prevtrig){
	//printf("CurrentTrig is: %i \n", trig0);
	//printf("Trigbuffer is: %s \n", trigBuffer);
	//printf("Error is: %i \n", error);
	//printf("From the source is: %i \n", horse);
	}
	prevtrig = trig;
	inty++;
	mWriteReg(MappedBaseAddress, RegOffset2,0);
	//sleep(1);
  }

  munmap((void*)MappedBaseAddress, (HighAddress-BaseAddress));
  munmap((void*)MappedBaseAddressX, (HighAddressX-BaseAddressX));
}

#endif /* TRIGGERS_H_ */
