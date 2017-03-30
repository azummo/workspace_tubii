/*
 * tubiiClocks.h
 *
 *  Created on: Mar 20, 2015
 *      Author: Ian
 */
#include "tubiiUtil.h"

#ifndef CLOCKS_H_
#define CLOCKS_H_

void *MappedClocksBaseAddress;

int clockReset(int clk_choice)
{
  if(clk_choice==1 || clk_choice==0){
	mWriteReg((u32) MappedClocksBaseAddress, RegOffset0, clk_choice);
  }
  else{
    Log(WARNING, "TUBii: Invalid clock choice. Choose 0 or 1.");
	sprintf(tubii_err, "TUBii: Invalid clock choice. Choose 0 or 1.");
	return -1;
  }

  return 0;
}

int clockStatus()
{
  return mReadReg((u32) MappedClocksBaseAddress, RegOffset1);
}

#endif /* CLOCKS_H_ */
