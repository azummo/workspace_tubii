/*
 * tubiiRegs.h
 *
 *  Created on: Mar 20, 2015
 *      Author: Ian
 */

#ifndef TUBIIREGS_H_
#define TUBIIREGS_H_

void *MappedRegsBaseAddress;
void *MappedReadBaseAddress;

int Muxer(u32 mux)
{
  if(mux>7 || mux<0){
	Log(WARNING, "TUBii: Muxer must be between 0 and 7.");
    sprintf(tubii_err, "TUBii: Muxer must be between 0 and 7.");
	return -1;
  }
  mWriteReg((u32) MappedRegsBaseAddress, RegOffset0, mux);

  return 0;
}

int MuxEnable(u32 mux)
{
  if(mux != 0 && mux != 1){
	Log(WARNING, "TUBii: MuxEnable must be 1 or 0.");
	sprintf(tubii_err, "TUBii: MuxEnable must be 1 or 0.");
	return -1;
  }
  mWriteReg((u32) MappedRegsBaseAddress, RegOffset1, mux);

  return 0;
}

int DataReady(u32 dReg)
{
  // 1 ControlReg
  // 2 CAEN
  // 3 MTCA Mimic
  // 4 Clocks
  if(dReg > 15 || dReg <0){
	Log(WARNING, "TUBii: Invalid register selected.");
	sprintf(tubii_err, "TUBii: Invalid register selected.");
	return -1;
  }
  mWriteReg((u32) MappedRegsBaseAddress, RegOffset2, dReg);

  return 0;
}

int LoadShift(u32 data)
{
  if(data>255 || data<0){
	Log(WARNING, "TUBii: Data out of range 0 to 255.");
	sprintf(tubii_err, "TUBii: Data out of range 0 to 255.");
	return -1;
  }
  mWriteReg((u32) MappedRegsBaseAddress, RegOffset3, data);

  int cnt=0;
  for(cnt=0; cnt<8; cnt++){
	// Set
	mWriteReg((u32) MappedRegsBaseAddress, RegOffset4, 1);
	usleep(10000);
  }

  return 0;
}

int ControlReg(int word)
{
  Muxer(0);
  MuxEnable(1);
  LoadShift(word);
  MuxEnable(0);
  DataReady(5);
  DataReady(4);

  mWriteReg((u32) MappedRegsBaseAddress, RegOffset10, word);
  Log(VERBOSE, "TUBii: Set control register %d.", word);

  return 0;
}


int CAENWords(int GainPath, int ChanSelect)
{
  Muxer(1);
  MuxEnable(1);
  LoadShift(GainPath);
  LoadShift(ChanSelect);
  MuxEnable(0);
  DataReady(6);
  DataReady(4);

  mWriteReg((u32) MappedRegsBaseAddress, RegOffset11, GainPath);
  mWriteReg((u32) MappedRegsBaseAddress, RegOffset12, ChanSelect);
  Log(VERBOSE, "TUBii: Set CAEN gain path %d.", GainPath);
  Log(VERBOSE, "TUBii: Set CAEN channel select %d.", ChanSelect);

  return 0;
}

int DACThresholds(int DACThresh)
{
  int DACThresh_pt1 = DACThresh & 0xFF;
  int DACThresh_pt2 = (DACThresh >> 8) & 0xFF;

  Muxer(2);
  MuxEnable(1);
  MuxEnable(0);
  LoadShift(DACThresh_pt2);
  LoadShift(DACThresh_pt1);
  MuxEnable(0);
  DataReady(0);
  DataReady(4);

  mWriteReg((u32) MappedRegsBaseAddress, RegOffset13, DACThresh);
  Log(VERBOSE, "TUBii: Set DAC threshold %d.", DACThresh);

  return 0;
}

int GTDelays(int LO, int DGT)
{
  Muxer(3);
  MuxEnable(1);
  LoadShift(LO);
  LoadShift(DGT);
  MuxEnable(0);

  mWriteReg((u32) MappedRegsBaseAddress, RegOffset14, LO);
  mWriteReg((u32) MappedRegsBaseAddress, RegOffset15, DGT);
  Log(VERBOSE, "TUBii: Set LO* Delay %d.", LO);
  Log(VERBOSE, "TUBii: Set DGT Delay %d.", DGT);

  return 0;
}

int ClockMisses(int nMisses)
{
  // I think this'll work...
  Muxer(5);
  MuxEnable(1);
  LoadShift(nMisses);
  MuxEnable(0);
  Log(VERBOSE, "TUBii: Set number of allowable missed clock ticks %d.", nMisses);

  return 0;
}

#endif /* TUBIIREGS_H_ */
