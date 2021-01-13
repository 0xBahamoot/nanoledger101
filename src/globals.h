#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include "os.h"
#include "ux.h"
#include "os_io_seproxyhal.h"
#include "key.h"

#define PIN_VERIFIED BOLOS_UX_OK
#define P1_CONFIRM 0x01
#define P1_NON_CONFIRM 0x00
#define P1_FIRST 0x00
#define P1_MORE 0x80

#define FULL_ADDRESS_LENGTH 54
#define BIP32_PATH 5

#define SW_WRONG_DATA_RANGE 0x6985
extern ux_state_t ux;
// display stepped screens
extern unsigned int ux_step;
extern unsigned int ux_step_count;
extern unsigned char processData[300];
extern unsigned char trust_host;

typedef struct internalStorage_t
{
    unsigned char setting_1;
    unsigned char setting_2;
    unsigned char setting_3;
    unsigned char setting_4;

    uint8_t initialized;
} internalStorage_t;

extern const internalStorage_t N_storage_real;
#define N_storage (*(volatile internalStorage_t *)PIC(&N_storage_real))

struct crypto_state_s
{
    uint8_t isHDGen;
    privatekey_t key;

    unsigned char a[32]; //private view key //this should be cal when needed?

    unsigned char A[32]; //public view key/transmission key //this should be cal when needed?
    unsigned char B[32]; //public spend key //this should be cal when needed?
    /* SPK */
    cx_aes_key_t spk;
    unsigned char hmac_key[32];

    /* Tx key */
    unsigned char R[32];
    unsigned char r[32];

    /* prefix/mlsag hash */
    cx_sha3_t keccakF;
    cx_sha3_t keccakH;
    // cx_sha3_t sha3;
    unsigned char prefixH[32];
    unsigned char mlsagH[32];
    unsigned char c[32];

    /* -- track tx-in/out and commitment -- */
    cx_sha256_t sha256_out_keys;
    unsigned char OUTK[32];

    cx_sha256_t sha256_commitment;
    unsigned char C[32];
};

typedef struct crypto_state_s crypto_state_t;

#define INCOGNITO_IO_BUFFER_LENGTH (300)
// struct io_state_s {
//     unsigned short io_length;
//     unsigned short io_offset;
//     unsigned char io_buffer[INCOGNITO_IO_BUFFER_LENGTH];
//     };

// typedef struct io_state_s io_state_t;
extern crypto_state_t G_crypto_state_t;
// extern  io_state_t G_io_state_t;
#endif
