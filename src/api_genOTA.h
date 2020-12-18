
#ifndef _GEN_OTA_H_
#define _GEN_OTA_H_
#include "os.h"
#include "cx.h"
#include "globals.h"

void handleGenOTA(uint8_t p1, uint8_t p2, uint8_t* dataBuffer, uint16_t dataLength, volatile unsigned int* flags, volatile unsigned int* tx);

#endif
