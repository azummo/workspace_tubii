#ifndef DATA_H
#define DATA_H

#include "ae.h"
#include "record_info.h"

int data_connect(const char *ip);
void write_data_buffer(aeEventLoop *el, int fd, void *data, int mask);
void write_to_data_stream(struct GenericRecordHeader *header, void *record);

#endif
