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

int LODelay;
int DGTDelay;
int GainPathWord;
int ChannelSelectWord;
int CntrlReg;
int DACThresh;

int Muxer(char* mArg)
{
  u32 mux= (u32) atoi(mArg);
  if(mux>7 || mux<0){
	Log(WARNING, "TUBii: Muxer must be between 0 and 7.");
    sprintf(tubii_err, "TUBii: Muxer must be between 0 and 7.");
	return -1;
  }
  mWriteReg(MappedRegsBaseAddress, RegOffset0, mux);
  Log(NOTICE, "TUBii: Choose Muxer %s.", mArg);

  return 0;
}

int MuxEnable(char* meArg)
{
  int mux= (int) atoi(meArg);
  if(mux != 0 && mux != 1){
	Log(WARNING, "TUBii: MuxEnable must be 1 or 0.");
	sprintf(tubii_err, "TUBii: MuxEnable must be 1 or 0.");
	return -1;
  }

  mWriteReg(MappedRegsBaseAddress, RegOffset1, mux);
  Log(NOTICE, "TUBii: MuxEnable set to %s.", meArg);

  return 0;
}

int DataReady(char* dArg)
{
  // 1 ControlReg
  // 2 CAEN
  // 3 MTCA Mimic
  // 4 Clocks
  int dReg = atoi(dArg);
  if(dReg > 15 || dReg <0){
	Log(WARNING, "TUBii: Invalid register selected.");
	sprintf(tubii_err, "TUBii: Invalid register selected.");
	return -1;
  }

  mWriteReg(MappedRegsBaseAddress, RegOffset2, dReg);

  return 0;
}

int LoadShift(u32 data)
{
  if(data>255 || data<0){
	Log(WARNING, "TUBii: Data out of range 0 to 255.");
	sprintf(tubii_err, "TUBii: Data out of range 0 to 255.");
	return -1;
  }
  mWriteReg(MappedRegsBaseAddress, RegOffset3, data);

  int cnt=0;
  for(cnt=0; cnt<8; cnt++){
	  // Set
	  mWriteReg(MappedRegsBaseAddress, RegOffset4, 1);
	  usleep(10000);
  }

  return 0;
}

int ReadShift()
{
  mWriteReg(MappedRegsBaseAddress, RegOffset7, 0);
  usleep(10000);
  mWriteReg(MappedRegsBaseAddress, RegOffset7, 2);
  usleep(10000);
  int cnt=0;
  for(cnt=0; cnt<8; cnt++){
	  // Set
	  mWriteReg(MappedRegsBaseAddress, RegOffset7, 3);
	  usleep(10000);
  }
  u32 word = mReadReg(MappedRegsBaseAddress, RegOffset6) & 0xFF;

  return word;
}

#endif /* TUBIIREGS_H_ */
