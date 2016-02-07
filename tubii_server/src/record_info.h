#ifndef __RECORD_INFO_H
#define __RECORD_INFO_H

#include <stdint.h>

#define RECORD_VERSION 0

struct GenericRecordHeader {
    uint32_t RecordID; // 4 bytes, in ascii, some run header
    uint32_t RecordLength; // number of bytes in record (not including header)
    uint32_t RecordVersion; // 0
};

struct TubiiRecord {
    uint32_t TrigWord;
    uint32_t GTID;
};

enum RecordTypes {
    RHDR_RECORD    = 0x52484452,
    EPED_RECORD    = 0x45504544,
    CAEN_RECORD    = 0x4341454e,
    MTCD_RECORD    = 0x4d544344,
    MEGA_BUNDLE    = 0x4d454741,
    MTCD_STATUS    = 0x4d545354,
    TRIG_RECORD    = 0x54524947,
    TUBII_RECORD   = 0xabc12345, //place holder
};



#endif
