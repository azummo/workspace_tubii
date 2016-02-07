/*
 * tubiiELLIE.h
 *
 *  Created on: Mar 20, 2015
 *      Author: Ian
 */
#include "tubiiUtil.h"

#ifndef TUBIIELLIE_H_
#define TUBIIELLIE_H_

int Pulser(char* argument, char* npArg, u32 BaseAddress, u32 HighAddress)
{
  void *MappedBaseAddress= MemoryMapping(BaseAddress,HighAddress);
  InitialiseRegs(MappedBaseAddress);

  // Convert from 100 MHz FClk
  double HunMHz = 50000000;
  double rate= atof(argument);
  rate = 1.0/rate;
  //if(rate < 0.01 || rate > 100) return -1;
  printf("Rate is %s Hz or %lf clock cycles\n",argument,rate);
  u32 period = 0.5*rate*HunMHz;
  u32 width = rate*HunMHz;
  int nPulse= atoi(npArg);

  // 0 is time between pulses, 1 is pulse width, 3 in no. of pulses
  mWriteReg(MappedBaseAddress, RegOffset0, period);
  mWriteReg(MappedBaseAddress, RegOffset1, width);
  mWriteReg(MappedBaseAddress, RegOffset3, nPulse);

  munmap((void*)MappedBaseAddress, (HighAddress-BaseAddress));

  return 0;
}

int Delay(char* argument, u32 BaseAddress, u32 HighAddress)
{
  void *MappedBaseAddress= MemoryMapping(BaseAddress,HighAddress);
  InitialiseRegs(MappedBaseAddress);

  double ddelay= (double) atof(argument);
  double ns = 0.1;
  if(ddelay<0) return -1;
  u32 delay = ddelay*ns;
  printf("Delay length = %d\n", delay);

  // Set Delay
  mWriteReg(MappedBaseAddress, RegOffset3, delay);

  munmap((void*)MappedBaseAddress, (HighAddress-BaseAddress));

  return 0;
}

#endif /* TUBIIELLIE_H_ */
