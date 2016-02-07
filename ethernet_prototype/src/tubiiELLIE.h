/*
 * tubiiELLIE.h
 *
 *  Created on: Mar 20, 2015
 *      Author: Ian
 */

#ifndef TUBIIELLIE_H_
#define TUBIIELLIE_H_

void Pulser(char* argument, u32 BaseAddress, u32 HighAddress)
{
  void *MappedBaseAddress= MemoryMapping(BaseAddress,HighAddress);
  InitialiseRegs(MappedBaseAddress);

  // Convert from 100 MHz FClk
  double HunMHz = 50000000;
  double rate= atof(argument);
  printf("Rate is %d Hz\n",rate);
  u32 period = 0.5*rate*HunMHz;
  u32 width = rate*HunMHz;

  // 0 is time between pulses, 1 is pulse width
  mWriteReg(MappedBaseAddress, RegOffset0, period);
  mWriteReg(MappedBaseAddress, RegOffset1, width);

  munmap((void*)MappedBaseAddress, (HighAddress-BaseAddress));
}

void Delay(char* argument, u32 BaseAddress, u32 HighAddress)
{
  void *MappedBaseAddress= MemoryMapping(BaseAddress,HighAddress);
  InitialiseRegs(MappedBaseAddress);

  double ddelay= (double) atof(argument);
  double HunMHz = 50000000;
  u32 delay = ddelay*HunMHz;
  printf("Delay length = %d\n", delay);

  // Set Delay
  mWriteReg(MappedBaseAddress, RegOffset3, delay);

  munmap((void*)MappedBaseAddress, (HighAddress-BaseAddress));
}

#endif /* TUBIIELLIE_H_ */
