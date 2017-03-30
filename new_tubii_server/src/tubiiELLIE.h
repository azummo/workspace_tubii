/*
 * tubiiELLIE.h
 *
 *  Created on: Mar 20, 2015
 *      Author: Ian
 */
#include "tubiiUtil.h"

#ifndef TUBIIELLIE_H_
#define TUBIIELLIE_H_

void *MappedGTDelayBaseAddress;
void *MappedDelayBaseAddress;
void *MappedSDelayBaseAddress;
void *MappedTDelayBaseAddress;
void *MappedHappyBaseAddress;
void *MappedPulserBaseAddress;
void *MappedSPulserBaseAddress;
void *MappedTPulserBaseAddress;
void *MappedDelayLengthenBaseAddress;

int Pulser(float rate, float length, u32 nPulse, void* MappedBaseAddress)
{
  if(rate < 0 || rate > 1000000){
	Log(WARNING, "TUBii: pulser rate is outside acceptable range.");
	sprintf(tubii_err, "Tubii: pulser rate is outside acceptable range.");
	return -1;
  }

  if(length < 0 ){
	Log(WARNING, "TUBii: Pulse width is too short.");
	sprintf(tubii_err, "Tubii: Pulse width is too short.");
	return -1;
  }
  else if(rate!=0 && length > 1.0/(rate) ){
	Log(WARNING, "TUBii: Pulse width is longer than period.");
	sprintf(tubii_err, "Tubii: Pulse width is longer than period.");
	return -1;
  }

  u32 period = HunMHz/rate;
  u32 width = period - length*HunMHz; // Due to bad planning, width is length of time pulse is low
  Log(VERBOSE, "TUBii: rate is %f Hz for %d pulses.", rate, nPulse);

  // 0 is pulse width, 1 is period, 3 is no. of pulses
  mWriteReg((u32) MappedBaseAddress, RegOffset0, width);
  mWriteReg((u32) MappedBaseAddress, RegOffset1, period);
  mWriteReg((u32) MappedBaseAddress, RegOffset3, nPulse);

  return 0;
}

int Delay(u32 delay, void* MappedBaseAddress)
{
  if(delay<0){
	Log(WARNING, "TUBii: delay length is outside acceptable range.");
	sprintf(tubii_err, "Tubii: delay length is outside acceptable range.");
	return -1;
  }

  // Set Delay
  mWriteReg((u32) MappedBaseAddress, RegOffset3, delay);
  Log(VERBOSE, "TUBii: delay length is %d ns.", delay);

  return 0;
}

int Lengthen(char* dArg)
{
  u32 length=0;
  safe_strtoul(dArg,&length);
  mWriteReg((u32) MappedDelayLengthenBaseAddress, RegOffset1, length);
  mWriteReg((u32) MappedDelayLengthenBaseAddress, RegOffset2, length);

  return 0;
}


#endif /* TUBIIELLIE_H_ */
