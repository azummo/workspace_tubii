#ifndef TUBII_CLIENT_H
#define TUBII_CLIENT_H

#include "server.h"
#include "sds.h"

int safe_strtoul(char *s, uint32_t *si);
int safe_strtoull(char *s, uint64_t *si);
int safe_strtof(char *s, float *f);

// Initialise
int auto_init();
void initialise(client *c, int argc, sds *argv);
void MZHappy(client *c, int argc, sds *argv);
void SetMZHappyPulser(client *c, int argc, sds *argv);
void ping(client *c, int argc, sds *argv);

// Clock
void clockreset(client *c, int argc, sds *argv);
void clockstatus(client *c, int argc, sds *argv);
void clockdebug(client *c, int argc, sds *argv);

// Low-level register commands
void dataready(client *c, int argc, sds *argv);
void loadShift(client *c, int argc, sds *argv);
void muxenable(client *c, int argc, sds *argv);
void muxer(client *c, int argc, sds *argv);

// LO & DGT Settings
void SetGTDelays(client *c, int argc, sds *argv);
void GetLODelay(client *c, int argc, sds *argv);
void GetDGTDelay(client *c, int argc, sds *argv);

// CAEN Settings
void SetCaenWords(client *c, int argc, sds *argv);
void GetCAENGainPathWord(client *c, int argc, sds *argv);
void GetCAENChannelSelectWord(client *c, int argc, sds *argv);

// Control Register
void SetControlReg(client *c, int argc, sds *argv);
void GetControlReg(client *c, int argc, sds *argv);

// DAC Settings
void SetDACThreshold(client *c, int argc, sds *argv);
void GetDACThreshold(client *c, int argc, sds *argv);

// Clock Misses
void SetAllowableClockMisses(client *c, int argc, sds *argv);

// Ellie Commands
void SetGenericdelay(client *c, int argc, sds *argv);
void SetGenericpulser(client *c, int argc, sds *argv);
void GetPulserRate(client *c, int argc, sds *argv);
void GetPulserWidth(client *c, int argc, sds *argv);
void GetPulserNPulses(client *c, int argc, sds *argv);
void GetDelay(client *c, int argc, sds *argv);
void SetSmelliedelay(client *c, int argc, sds *argv);
void SetSmelliepulser(client *c, int argc, sds *argv);
void GetSmellieRate(client *c, int argc, sds *argv);
void GetSmelliePulseWidth(client *c, int argc, sds *argv);
void GetSmellieNPulses(client *c, int argc, sds *argv);
void GetSmellieDelay(client *c, int argc, sds *argv);
void SetTelliedelay(client *c, int argc, sds *argv);
void SetTelliepulser(client *c, int argc, sds *argv);
void GetTellieRate(client *c, int argc, sds *argv);
void GetTelliePulseWidth(client *c, int argc, sds *argv);
void GetTellieNPulses(client *c, int argc, sds *argv);
void GetTellieDelay(client *c, int argc, sds *argv);

// Trigger Commands
void SetCounterMask(client *c, int argc, sds *argv);
void GetCounterMask(client *c, int argc, sds *argv);
void SetSpeakerMask(client *c, int argc, sds *argv);
void GetSpeakerMask(client *c, int argc, sds *argv);
void SetTriggerMask(client *c, int argc, sds *argv);
void GetTriggerMask(client *c, int argc, sds *argv);
void countLatch(client *c, int argc, sds *argv);
void countReset(client *c, int argc, sds *argv);
void gtdelay(client *c, int argc, sds *argv);
void SetBurstTrigger(client *c, int argc, sds *argv);
void SetComboTrigger(client *c, int argc, sds *argv);
void SetPrescaleTrigger(client *c, int argc, sds *argv);
void GetCurrentTrigger(client *c, int argc, sds *argv);
void GetFifoTrigger(client *c, int argc, sds *argv);

// TUBii Readout
void start_data_readout(client *c, int argc, sds *argv);
void stop_data_readout(client *c, int argc, sds *argv);
int tubii_status(aeEventLoop *el, long long id, void *data);
int tubii_readout(aeEventLoop *el, long long id, void *data);
int start_tubii_readout(long long milliseconds);

#endif
