/*
 * tubiiRegs.h
 *
 *  Created on: Mar 20, 2015
 *      Author: Ian
 */

#ifndef TUBIIREGS_H_
#define TUBIIREGS_H_

void Muxer(char* argument)
{
  u32 BaseAddress= MUXERINPUT_BASEADDR;
  u32 HighAddress= MUXERINPUT_HIGHADDR;
  void *MappedBaseAddress= MemoryMapping(BaseAddress,HighAddress);
  InitialiseRegs(MappedBaseAddress);

  u32 mux= (u32) atoi(argument);
  mWriteReg(MappedBaseAddress, RegOffset0, mux);
  printf("Choose Muxer: %x\n",  mReadReg(MappedBaseAddress, RegOffset0));

  munmap((void*)MappedBaseAddress, (HighAddress-BaseAddress));
}

void LoadShift(char* argument)
{
  u32 BaseAddress= LOADSHIFT_BASEADDR;
  u32 HighAddress= LOADSHIFT_HIGHADDR;
  void *MappedBaseAddress= MemoryMapping(BaseAddress,HighAddress);
  InitialiseRegs(MappedBaseAddress);

  u32 data= (u32) atoi(argument);
  mWriteReg(MappedBaseAddress, RegOffset0, data);
  printf("Write Data: %x to shift register.\n",  mReadReg(MappedBaseAddress, RegOffset0));

  int cnt=0;
  for(cnt=0; cnt<8; cnt++){
	  // Set
	  mWriteReg(MappedBaseAddress, RegOffset1, 1);
	  sleep(1);
  }

  munmap((void*)MappedBaseAddress, (HighAddress-BaseAddress));
}

#endif /* TUBIIREGS_H_ */
