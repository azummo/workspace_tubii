/* tubiiUtil.h
 *
 *  Created on: Apr 9, 2014
 *      Author: Ian Coulter (help from various sources)
 */
#include "sys/mman.h"
#include <fcntl.h>

#ifndef TUBIIUTIL_H_
#define TUBIIUTIL_H_

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

#define REG0_OFFSET 0
#define REG1_OFFSET 4
#define REG2_OFFSET 8
#define REG3_OFFSET 12
#define REG4_OFFSET 16
#define REG5_OFFSET 20
#define REG6_OFFSET 24
#define REG7_OFFSET 28

#define SOCKET_ERROR        -1
#define BUFFER_SIZE         100
#define QUEUE_SIZE          5

/** Write a value to a register. A 32 bit write is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is written.
 *
 * @param   BaseAddress is the base address of the CLOCKLOGICdevice.
 * @param   RegOffset is the register offset from the base to write to.
 * @param   Data is the data written to the register.
 *
 * @return  None.                                                     **/
#define mWriteReg(BaseAddress, RegOffset, Data) \
  	Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))

/** Read a value from a register. A 32 bit read is performed.
 * If the component is implemented in a smaller width, only the least
 * significant data is read from the register. The most significant data
 * will be read as 0.
 *
 * @param   BaseAddress is the base address of the CLOCKLOGIC device.
 * @param   RegOffset is the register offset from the base to write to.
 *
 * @return  Data is the data from the register.                       **/
#define mReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))

/// Register offsets
u32 RegOffset0= REG0_OFFSET;
u32 RegOffset1= REG1_OFFSET;
u32 RegOffset2= REG2_OFFSET;
u32 RegOffset3= REG3_OFFSET;
u32 RegOffset4= REG4_OFFSET;
u32 RegOffset5= REG5_OFFSET;
u32 RegOffset6= REG6_OFFSET;
u32 RegOffset7= REG7_OFFSET;

/// Read to and from addresses
u32 Xil_In32(u32 Addr)
{
  return *(volatile u32 *) Addr;
}

void Xil_Out32(u32 OutAddress, u32 Value)
{
  *(volatile u32 *) OutAddress = Value;
}

/// Memory Mapping Method
void* MemoryMapping(u32 BaseAddress, u32 HighAddress)
{
  //Begin memory mapping
  int memfd;
  void *MappedBaseAddress;
  off_t dev_base = BaseAddress;

  // Open Memory location
  memfd = open("/dev/mem", O_RDWR | O_SYNC);
  if (memfd == -1){
   	printf("Can't open /dev/mem.\n");
   	exit(0);
  }

  // Map into user space the area of memory containing the device
  MappedBaseAddress = mmap(0, (HighAddress-BaseAddress), PROT_READ | PROT_WRITE, MAP_SHARED, memfd, dev_base & ~(HighAddress-BaseAddress-1));
  if (MappedBaseAddress == (void *) -1){
   	printf("Can't map the memory to user space.\n");
    exit(0);
  }

  return MappedBaseAddress;
}

void InitialiseRegs(void* MappedBaseAddress)
{
  // Set regs to zero...
  mWriteReg(MappedBaseAddress, RegOffset0, 0);
  mWriteReg(MappedBaseAddress, RegOffset1, 0);
  mWriteReg(MappedBaseAddress, RegOffset2, 0);
  mWriteReg(MappedBaseAddress, RegOffset3, 0);
}

int Testing(u32 BaseAddress, u32 HighAddress)
{
	void* MappedBaseAddress=MemoryMapping(BaseAddress, HighAddress);
	printf("%x -> %x.\n",BaseAddress, MappedBaseAddress);
	sleep(1);
	InitialiseRegs(MappedBaseAddress);
	return 0;
}

#endif /* TUBIIUTIL_H_ */
