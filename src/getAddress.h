
#ifndef _GET_ADDRESS_H_
#define _GET_ADDRESS_H_
#include "os.h"
#include "cx.h"
#include "globals.h"

void handleGetAddress(uint8_t p1, uint8_t p2, uint8_t *dataBuffer, uint16_t dataLength, volatile unsigned int *flags, volatile unsigned int *tx);

#endif
