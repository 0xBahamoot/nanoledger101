
#ifndef _KEY_H_
#define _KEY_H_
#include "os.h"
#include "cx.h"
#include "globals.h"

struct privatekey
    {
    unsigned char depth;
    uint32_t child_number;
    unsigned char chain_code[32];
    unsigned char key[32];
    //total of 69 bytes
    };


typedef struct privatekey privatekey_t;

void incognito_init_private_key();
void incognito_reset_crypto_state();
void incognito_export_private_key();

#endif