/*
 * tubiiRegs.h
 *
 *  Created on: Mar 20, 2015
 *      Author: Ian
 */

#ifndef TUBIIREGS_H_
#define TUBIIREGS_H_

int Muxer(char* argument)
{
  u32 BaseAddress= SHIFTREGS_BASEADDR;
  u32 HighAddress= SHIFTREGS_HIGHADDR;
  void *MappedBaseAddress= MemoryMapping(BaseAddress,HighAddress);
  //InitialiseRegs(MappedBaseAddress);

  u32 mux= (u32) atoi(argument);
  if(mux>7 || mux<0) return -1;
  mWriteReg(MappedBaseAddress, RegOffset0, mux);
  printf("Choose Muxer: %x\n",  mReadReg(MappedBaseAddress, RegOffset0));

  munmap((void*)MappedBaseAddress, (HighAddress-BaseAddress));

  return 0;
}

int MuxEnable(char* argument)
{
  u32 BaseAddress= SHIFTREGS_BASEADDR;
  u32 HighAddress= SHIFTREGS_HIGHADDR;
  void *MappedBaseAddress= MemoryMapping(BaseAddress,HighAddress);
  //InitialiseRegs(MappedBaseAddress);

  int mux= (int) atoi(argument);
  if(mux != 0 && mux != 1) return -1;

  mWriteReg(MappedBaseAddress, RegOffset1, mux);
  printf("Choose MuxEnable: %x\n",  mReadReg(MappedBaseAddress, RegOffset1));

  munmap((void*)MappedBaseAddress, (HighAddress-BaseAddress));

  return 0;
}

int DataReady(char* argument)
{
  u32 BaseAddress= SHIFTREGS_BASEADDR;
  u32 HighAddress= SHIFTREGS_HIGHADDR;
  void *MappedBaseAddress= MemoryMapping(BaseAddress,HighAddress);

//  InitialiseRegs(MappedBaseAddress);

  /*if(argument == "ControlReg")
  	mWriteReg(MappedBaseAddress, RegOffset0, 1);
  else if(argument == "CAEN")
    mWriteReg(MappedBaseAddress, RegOffset0, 2);
  else if(argument == "MTCAMimic")
    mWriteReg(MappedBaseAddress, RegOffset0, 4);
  else if(argument == "Clocks")
    mWriteReg(MappedBaseAddress, RegOffset0, 8);
  else
	return -1;*/
  int arg = atoi(argument);
  /*printf("Write %i to the register\n", arg);
  if(arg == 0)
  	mWriteReg(MappedBaseAddress, RegOffset0, 1);
  else if(arg == 1)
    mWriteReg(MappedBaseAddress, RegOffset0, 2);
  else if(arg == 2)
    mWriteReg(MappedBaseAddress, RegOffset0, 4);
  else if(arg == 3)
    mWriteReg(MappedBaseAddress, RegOffset0, 8);
  else
	return -1;
  sleep(0.1);*/
  printf("Write %i to the register\n", arg);
  mWriteReg(MappedBaseAddress, RegOffset2, arg);

  munmap((void*)MappedBaseAddress, (HighAddress-BaseAddress));

  return 0;
}

int LoadShift(char* argument)
{
  u32 BaseAddress= SHIFTREGS_BASEADDR;
  u32 HighAddress= SHIFTREGS_HIGHADDR;
  void *MappedBaseAddress= MemoryMapping(BaseAddress,HighAddress);
  //InitialiseRegs(MappedBaseAddress);

  u32 data= (u32) atoi(argument);
  if(data>255 || data<0) return -1;
  mWriteReg(MappedBaseAddress, RegOffset3, data);
  printf("Write Data: %x to shift register.\n",  mReadReg(MappedBaseAddress, RegOffset3));

  int cnt=0;
  for(cnt=0; cnt<8; cnt++){
	  // Set
	  mWriteReg(MappedBaseAddress, RegOffset4, 1);
	  usleep(10000);
  }

  munmap((void*)MappedBaseAddress, (HighAddress-BaseAddress));

  return 0;
}

int ReadShift()
{
  u32 BaseAddress= READSHIFT_BASEADDR;
  u32 HighAddress= READSHIFT_HIGHADDR;
  void *MappedBaseAddress= MemoryMapping(BaseAddress,HighAddress);
  //InitialiseRegs(MappedBaseAddress);

  int cnt=0;
  for(cnt=0; cnt<8; cnt++){
	  // Set
	  mWriteReg(MappedBaseAddress, RegOffset1, 1);
	  usleep(10000);
  }
  u32 word = mReadReg(MappedBaseAddress, RegOffset0);
  printf("Read Data: %x from shift register.\n",  word);

  munmap((void*)MappedBaseAddress, (HighAddress-BaseAddress));

  return word;
}

#endif /* TUBIIREGS_H_ */
