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

int smrate, smwidth, smnpulse, smdelay;
int trate, twidth, tnpulse, tdelay;
int grate, gwidth, gnpulse, gdelay;

int Pulser(char* rArg, char* wArg, char* npArg, void* MappedBaseAddress)
{
  InitialiseRegs(MappedBaseAddress);

  // Convert from 100 MHz Clk
  double HunMHz = 100000000;
  double rate= atof(rArg);
  double length= atof(wArg);

  if(rate < 0.001 || rate > 1000000){
	Log(WARNING, "TUBii: pulser rate is outside acceptable range.");
	sprintf(tubii_err, "Tubii: pulser rate is outside acceptable range.");
	return -1;
  }

  if(length <= 0 ){
	Log(WARNING, "TUBii: Pulse width is too short.");
	sprintf(tubii_err, "Tubii: Pulse width is too short.");
	return -1;
  }
  else if(length > 1.0/rate ){
	Log(WARNING, "TUBii: Pulse width is longer than period.");
	sprintf(tubii_err, "Tubii: Pulse width is longer than period.");
	return -1;
  }

  rate = 1.0/rate;
  u32 period = rate*HunMHz;
  u32 width = period - length*HunMHz; // Due to bad planning, width is length of time pulse is low

  int nPulse= atoi(npArg);
  Log(NOTICE, "TUBii: rate is %s Hz for %s pulses.", rArg, npArg);

  // 0 is pulse width, 1 is period, 3 is no. of pulses
  mWriteReg(MappedBaseAddress, RegOffset0, width);
  mWriteReg(MappedBaseAddress, RegOffset1, period);
  mWriteReg(MappedBaseAddress, RegOffset3, nPulse);

  return 0;
}

int Delay(char* dArg, void* MappedBaseAddress)
{
  InitialiseRegs(MappedBaseAddress);

  double ns = 0.1;
  double length= (double) atof(dArg);
  u32 delay = length*ns;
  if(delay<0){
	Log(WARNING, "TUBii: delay length is outside acceptable range.");
	sprintf(tubii_err, "Tubii: delay length is outside acceptable range.");
	return -1;
  }

  Log(NOTICE, "TUBii: delay length is %s ns.", dArg);

  // Set Delay
  mWriteReg(MappedBaseAddress, RegOffset3, delay);

  return 0;
}

int Lengthen(char* dArg)
{
	int length= atoi(dArg);
	mWriteReg(MappedDelayLengthenBaseAddress, RegOffset1, length);
	mWriteReg(MappedDelayLengthenBaseAddress, RegOffset2, length);

	return 0;
}


#endif /* TUBIIELLIE_H_ */
