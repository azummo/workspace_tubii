#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "sys/mman.h"
#include <fcntl.h>
#include <pthread.h>

/// Tubii Headers
#include "tubiiAddresses.h"
#include "tubiiSockets.h"
#include "tubiiUtil.h"

#include "tubiiELLIE.h"
#include "tubiiRegs.h"
#include "tubiiTriggers.h"

/// Prototypes
////////////////////////////////////////////////////////////////////

/// Socket functions
int MakeSocket();
struct sockaddr_in GetAddress(int nHostPort);
int BindSocket(int hServerSocket, struct sockaddr_in Address);
int ConnectSocket(int hServerSocket, struct sockaddr_in Address);
int RecieveCommand(int hSocket, char* tubBuffer, char** command, char** argument);
int HandShaking(int hSocket);

/// General Utilities
void* MemoryMapping(u32 base, u32 high);
void InitialiseRegs(void* MappedBaseAddress);

/// Generic ELLIE functions
void Pulser(char* argument, u32 BaseAddress, u32 HighAddress);
void Delay(char* argument, u32 BaseAddress, u32 HighAddress);

/// Command reg functions
void Muxer(char* argument);
void LoadShift(char* argument);
void countDisplay(char* argument);
void backupClock(char* argument);

/// Trigger functions
void burstTrig(char* argument);
void ComboTrig(char* argument);
void FanInTrig(char* argument);
void PrescaleTrig(char* argument);
void triggerReport(void* arg);

////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
  int hSocketReceive, hServerSocketReceive;  // handle to receiving socket
  struct sockaddr_in AddressReceive;         // Internet socket address stuct (receiving)
  int hSocketSend, hServerSocketSend;        // handle to sending socket
  struct sockaddr_in AddressSend;            // Internet socket address stuct (sending)
  char tubBuffer[BUFFER_SIZE];
  int nHostPort;

  /// Check input arguments
  if(argc < 2){
    printf("Usage: server host-port\n");
    return 0;
  }
  else{
    nHostPort=atoi(argv[1]);
  }

  printf("Starting server\n");
  hServerSocketReceive=MakeSocket();
  if(hServerSocketReceive < 0) return 0;
  AddressReceive= GetAddress(nHostPort);
  if(BindSocket(hServerSocketReceive, AddressReceive) < 0) return 0;

  hServerSocketSend=MakeSocket();
  if(hServerSocketSend < 0) return 0;
  AddressSend= GetAddress(nHostPort+1);
  if(BindSocket(hServerSocketSend, AddressSend) < 0) return 0;

  pthread_t tid;
  hSocketSend=ConnectSocket(hServerSocketSend, AddressSend);
  pthread_create(&tid, NULL, triggerReport, hSocketSend);

  triggerEnable(255);

  hSocketReceive=ConnectSocket(hServerSocketReceive, AddressReceive);
  for(;;)
    {
      /// Get the connected socket

      /// Check we can communicate
      //int shakey= HandShaking(hSocket);
      //if(shakey==-1) return -1;

      /// Read commands for tubii from socket into buffer
      char* command= (char*) malloc(BUFFER_SIZE);
      char* argument= (char*) malloc(BUFFER_SIZE);
      printf("Ready to recieve\n");
      RecieveCommand(hSocketReceive, tubBuffer, &command, &argument);

      /// Interpret commands
      if(!strcmp(command,"syncpulser")){
    	Pulser(argument,SYNCPULSER_BASEADDR,SYNCPULSER_HIGHADDR);
      }
      else if(!strcmp(command,"syncdelay")){
  	    Delay(argument,SYNCDELAY_BASEADDR,SYNCDELAY_HIGHADDR);
      }
      else if(!strcmp(command,"asyncpulser")){
    	// Need to split argument into the sync part and async part
    	// Sync part:
    	Pulser(argument,ASYNCPULSER_BASEADDR,ASYNCPULSER_HIGHADDR);
    	// Send async part to shift reg:
    	Muxer("4");
    	LoadShift(argument);
      }
      else if(!strcmp(command,"asyncdelay")){
      	// Need to split argument into the sync part and async part
      	// Sync part:
    	Delay(argument,ASYNCDELAY_BASEADDR,ASYNCDELAY_HIGHADDR);
    	// Send async part to shift reg:
    	Muxer("5");
    	LoadShift(argument);
      }
      else if(!strcmp(command,"smelliepulser")){
    	Pulser(argument,SMELLIEPULSER_BASEADDR,SMELLIEPULSER_HIGHADDR);
      }
      else if(!strcmp(command,"smelliedelay")){
    	Delay(argument,SMELLIEDELAY_BASEADDR,SMELLIEDELAY_HIGHADDR);
      }
      else if(!strcmp(command,"telliepulser")){
    	Pulser(argument,TELLIEPULSER_BASEADDR,TELLIEPULSER_HIGHADDR);
      }
      else if(!strcmp(command,"telliedelay")){
    	Delay(argument,TELLIEDELAY_BASEADDR,TELLIEDELAY_HIGHADDR);
      }
      else if(!strcmp(command,"trigBurst")){
    	burstTrig(argument);
      }
      else if(!strcmp(command,"trigCombo")){
    	ComboTrig(argument);
      }
      else if(!strcmp(command,"nothing")){
    	Nothing(argument);
      }
      else if(!strcmp(command,"trigFanIn")){
        FanInTrig(argument);
      }
      else if(!strcmp(command,"trigPrescale")){
    	PrescaleTrig(argument);
      }
      else if(!strcmp(command,"clock")){
        backupClock(argument);
      }
      else if(!strcmp(command,"countDisplay")){
    	countDisplay(argument);
      }
      else if(!strcmp(command,"loadShift")){
    	printf("JUST FOR DEBUGGING\n\n");
    	LoadShift(argument);
      }
      else if(!strcmp(command,"muxer")){
    	printf("JUST FOR DEBUGGING\n\n");
    	Muxer(argument);
      }
      else if(!strcmp(command,"help")){
    	printf("Valid options include:\n %s-X\n %s-X\n %s-X\n %s-X\n %s-X\n %s-X\n %s-X\n %s-X\n",
    		   "asyncpulser","asyncdelay","syncpulser","syncdelay","smelliepulser","smelliedelay","telliepulser","telliedelay");
    	printf("%s-X\n %s-X\n %s-X\n %s-X\n where X is some argument.\n\n",
    		   "trigBurst","trigCombo","trigFanIn","trigPrescale");
      }
      else{
    	printf("Invalid tubii command %s\n",command);
    	printf("Valid options include:\n %s-X\n %s-X\n %s-X\n %s-X\n %s-X\n %s-X\n %s-X\n %s-X\n",
    		   "asyncpulser","asyncdelay","syncpulser","syncdelay","smelliepulser","smelliedelay","telliepulser","telliedelay");
    	printf("%s-X\n %s-X\n %s-X\n %s-X\n where X is some argument.\n\n",
    		   "trigBurst","trigCombo","trigFanIn","trigPrescale");
      }

      tubBuffer[0] = '\0';
      /*printf("Closing the socket\n");
      if(close(hSocketReceive) == SOCKET_ERROR)
        {
	      printf("Could not close socket\n");
	      return 0;
        }*/
    }
}

void backupClock(char* argument){
  u32 BaseAddress= CLOCKLOGIC_BASEADDR;
  u32 HighAddress= CLOCKLOGIC_HIGHADDR;
  void *MappedBaseAddress= MemoryMapping(BaseAddress,HighAddress);

  int clk_choice= atoi(argument);
  if(clk_choice==1 || clk_choice==0) printf("Clock choice = %i\n", clk_choice);
  else{
    printf("Invalid clock choice. Choose 0 or 1.\n");
    return;
  }

  /// Write to the register 0, indicates which clock we want to use
  mWriteReg(MappedBaseAddress, RegOffset0, clk_choice);
  printf("Choose clock: %d\n",  mReadReg(MappedBaseAddress, clk_choice));

  munmap((void*)MappedBaseAddress, (HighAddress-BaseAddress));
}

void countDisplay(char* argument){
  u32 BaseAddress= COUNTDISP_BASEADDR;
  u32 HighAddress= COUNTDISP_HIGHADDR;
  void *MappedBaseAddress= MemoryMapping(BaseAddress,HighAddress);
  InitialiseRegs(MappedBaseAddress);

  u32 length= (u32) atoi(argument);
  mWriteReg(MappedBaseAddress, RegOffset3, length);

  while(1){
	printf("%x \t %x\n", mReadReg(MappedBaseAddress, RegOffset0), mReadReg(MappedBaseAddress, RegOffset3));
  }

  munmap((void*)MappedBaseAddress, (HighAddress-BaseAddress));
}
