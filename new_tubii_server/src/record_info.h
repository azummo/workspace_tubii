#ifndef __RECORD_INFO_H
#define __RECORD_INFO_H

#include <stdint.h>

#define RECORD_VERSION 0

struct GenericRecordHeader {
    uint32_t RecordID;
    uint32_t RecordLength;
    uint32_t RecordVersion;
};

struct TubiiRecord {
    uint32_t TrigWord;
    uint32_t GTID;
};

struct MegaRecord {
	struct TubiiRecord array[1000];
	int size;
};

struct TubiiStatus {
    uint32_t Clock;
    uint32_t ControlReg;
};

enum RecordTypes {
    RHDR_RECORD    = 0x52484452,
    EPED_RECORD    = 0x45504544,
    CAEN_RECORD    = 0x4341454e,
    MTCD_RECORD    = 0x4d544344,
    MEGA_BUNDLE    = 0x4d454741,
    MTCD_STATUS    = 0x4d545354,
    TRIG_RECORD    = 0x54524947,
    FIFO_LEVELS    = 0x4649464f,
    TUBII_RECORD   = 0xabc12345, // Temp
    TUBII_STATUS   = 0xabcd9876, // Temp
    MEGA_RECORD    = 0x54554232, // TUB2 Change later
};

#endif
