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

int clockReset(char* cArg)
{
	int clk_choice= atoi(cArg);
	int ret=0;
	if(clk_choice==1 || clk_choice==0){
	  mWriteReg(MappedClocksBaseAddress, RegOffset0, clk_choice);
	}
	else{
      Log(WARNING, "TUBii: Invalid clock choice. Choose 0 or 1.");
	  sprintf(tubii_err, "TUBii: Invalid clock choice. Choose 0 or 1.");
	  ret = -1;
	}

	return ret;
}

int clockStatus()
{
	int status=mReadReg(MappedClocksBaseAddress, RegOffset1);

	return status;
}

int clockDebug()
{
	u32 BaseAddress= COUNTDEBUG_BASEADDR;
	u32 HighAddress= COUNTDEBUG_HIGHADDR;
	void *MappedBaseAddress= MemoryMapping(BaseAddress,HighAddress);

	printf("%x\n",mReadReg(MappedBaseAddress, RegOffset0));

	munmap((void*)MappedBaseAddress, (HighAddress-BaseAddress));
	return 0;
}

#endif /* CLOCKS_H_ */
