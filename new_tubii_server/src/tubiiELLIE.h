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
  else if(rate!=0 && length/(1.0e9) > 1.0/(rate) ){
	Log(WARNING, "TUBii: Pulse width is longer than period.");
	sprintf(tubii_err, "Tubii: Pulse width is longer than period.");
	return -1;
  }

  u32 period = HunMHz/rate;
  if(rate==0) period=0;

  u32 width = period - length*ns; // Due to bad planning, width is length of time pulse is low
  Log(VERBOSE, "TUBii: rate is %f Hz for %d pulses.", rate, nPulse);

  // 0 is pulse width, 1 is period, 3 is no. of pulses
  mWriteReg((u32) MappedBaseAddress, RegOffset0, width);
  mWriteReg((u32) MappedBaseAddress, RegOffset1, period);
  Log(WARNING,"period %d\n",mReadReg((u32) MappedBaseAddress, RegOffset1));
  mWriteReg((u32) MappedBaseAddress, RegOffset3, nPulse);

  return 0;
}

double GetRate(void* MappedBaseAddress)
{
  u32 period= mReadReg((u32) MappedBaseAddress, RegOffset1);
  Log(WARNING,"period %d\n",period);
  float rate= HunMHz/period;
  Log(WARNING,"rate %f\n",rate);
  if(period==0) rate=0;

  return rate;
}

double GetWidth(void* MappedBaseAddress)
{
  float width= (mReadReg((u32) MappedBaseAddress, RegOffset1) - mReadReg((u32) MappedBaseAddress, RegOffset0));
  width *= 1/ns;

  return width;
}

int GetNPulses(void* MappedBaseAddress)
{
  return mReadReg((u32) MappedBaseAddress, RegOffset3);
}

int Delay(u32 delay, void* MappedBaseAddress)
{
  delay *= ns;

  if(delay<0){
	Log(WARNING, "TUBii: delay length is outside acceptable range.");
	sprintf(tubii_err, "Tubii: delay length is outside acceptable range.");
	return -1;
  }

  // Set Delay
  mWriteReg((u32) MappedBaseAddress, RegOffset3, delay);
  return 0;
}

int GetDelayLength(void* MappedBaseAddress)
{
  return mReadReg((u32) MappedBaseAddress, RegOffset3)/ns;
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
