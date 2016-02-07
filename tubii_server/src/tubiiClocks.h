/*
 * tubiiClocks.h
 *
 *  Created on: Mar 20, 2015
 *      Author: Ian
 */
#include "tubiiUtil.h"

#ifndef CLOCKS_H_
#define CLOCKS_H_

int clockReset(char* argument)
{
	u32 BaseAddress= CLOCKLOGIC_BASEADDR;
	u32 HighAddress= CLOCKLOGIC_HIGHADDR;
	void *MappedBaseAddress= MemoryMapping(BaseAddress,HighAddress);

	int clk_choice= atoi(argument);
	if(clk_choice==1 || clk_choice==0){
	  mWriteReg(MappedBaseAddress, RegOffset0, clk_choice);
	  munmap((void*)MappedBaseAddress, (HighAddress-BaseAddress));
	  return 0;
	}
	else{
      munmap((void*)MappedBaseAddress, (HighAddress-BaseAddress));
	  return clk_choice;
	}
}

int clockStatus()
{
	u32 BaseAddress= CLOCKLOGIC_BASEADDR;
	u32 HighAddress= CLOCKLOGIC_HIGHADDR;
	void *MappedBaseAddress= MemoryMapping(BaseAddress,HighAddress);

	int status=mReadReg(MappedBaseAddress, RegOffset1);

	munmap((void*)MappedBaseAddress, (HighAddress-BaseAddress));
	return status;
}

#endif /* CLOCKS_H_ */
