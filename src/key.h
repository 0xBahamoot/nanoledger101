
#ifndef _KEY_H_
#define _KEY_H_
#include "os.h"
#include "cx.h"
#include "globals.h"

struct privatekey
{
    unsigned char key[32];        //256bits
    unsigned char chain_code[32]; //256bits
    unsigned char depth;          //8bits
    uint32_t child_number;        //32bits
    //total of 69 bytes

    //todo combine depth & child_number for mm align
};

typedef struct privatekey privatekey_t;

void incognito_init_private_key();
void incognito_reset_crypto_state();
void incognito_init_crypto_state();
void incognito_gen_public_view_key(unsigned char *key);
void incognito_gen_public_spend_key(unsigned char *key);
void incognito_gen_private_ota_key(unsigned char *key);
void incognito_gen_public_ota_key(unsigned char *key);
#endif