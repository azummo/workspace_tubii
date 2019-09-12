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
void *MappedTUBiiClockBaseAddress;
void *MappedMZClockBaseAddress;
void *MappedClockCompBaseAddress;

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

int getClockTicks(int delay, void* MappedBaseAddress)
{
  if( delay == 0 )
  {
    return mReadReg((u32) MappedBaseAddress, RegOffset0);
  }
  else
  {
	u32 counts_init = mReadReg((u32) MappedBaseAddress, RegOffset0);
	usleep( delay );
	u32 counts_fin = mReadReg((u32) MappedBaseAddress, RegOffset0);
    return counts_fin - counts_init;
  }
}
/*
int getMZClockTicks()
{
  return mReadReg((u32) MappedMZClockBaseAddress, RegOffset0);
}

int getClockTickDiff(delay)
{
  u32 diff_init = mReadReg((u32) MappedClockCompBaseAddress, RegOffset0);
  usleep(1000);
  u32 diff_fin = mReadReg((u32) MappedClockCompBaseAddress, RegOffset0);
  if( diff_fin > diff_init) return diff_fin - diff_init;
  else return -(diff_init-diff_fin);
}
*/
#endif /* CLOCKS_H_ */
