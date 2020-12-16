
#ifndef INCOGNITO_TYPES_H
#define INCOGNITO_TYPES_H

#include "os_io_seproxyhal.h"

#define STATE_IDLE 0xC0

#define TYPE_SCALAR     1

#if CX_APILEVEL == 8
#define PIN_VERIFIED (!0)
#elif CX_APILEVEL >= 9

#define PIN_VERIFIED BOLOS_UX_OK
#else
#error CX_APILEVEL not  supported
#endif

#define INCOGNITO_IO_BUFFER_LENGTH (300)
#define INCOGNITO_APDU_LENGTH 0xFE

struct incognito_nv_state_s {
    /* magic */
    unsigned char magic[8];

/* key mode */
#define KEY_MODE_EXTERNAL 0x21
#define KEY_MODE_SEED     0x42
    unsigned char key_mode;

    /* acount id for bip derivation */
    unsigned int account_id;

    /* spend key */
    unsigned char b[32];
    /* view key */
    unsigned char a[32];

/*words*/
#define WORDS_MAX_LENGTH 20
    union {
        char words[26][WORDS_MAX_LENGTH];
        char words_list[25 * WORDS_MAX_LENGTH + 25];
    };
};

typedef struct incognito_nv_state_s incognito_nv_state_t;


struct incognito_v_state_s {
    unsigned char state;
    unsigned char protocol;

    /* ------------------------------------------ */
    /* ---                  IO                --- */
    /* ------------------------------------------ */

    /* io state*/
    unsigned char io_protocol_version;
    unsigned char io_ins;
    unsigned char io_p1;
    unsigned char io_p2;
    unsigned char io_lc;
    unsigned char io_le;
    unsigned short io_length;
    unsigned short io_offset;
    unsigned short io_mark;
    unsigned char io_buffer[INCOGNITO_IO_BUFFER_LENGTH];

    unsigned int options;

    /* ------------------------------------------ */
    /* ---            State Machine           --- */
    /* ------------------------------------------ */
    unsigned int export_view_key;
    unsigned char key_set;

/* protocol guard */
#define PROTOCOL_LOCKED            0x42
#define PROTOCOL_LOCKED_UNLOCKABLE 0x84
#define PROTOCOL_UNLOCKED          0x24
    unsigned char protocol_barrier;

    /* Tx state machine */
    unsigned char tx_in_progress;
    unsigned char tx_cnt;
    unsigned char tx_sig_mode;
    unsigned char tx_state_ins;
    unsigned char tx_state_p1;
    unsigned char tx_state_p2;
    unsigned char tx_output_cnt;
    unsigned int tx_sign_cnt;

    /* sc_add control */
    unsigned char last_derive_secret_key[32];
    unsigned char last_get_subaddress_secret_key[32];

    /* ------------------------------------------ */
    /* ---               Crypo                --- */
    /* ------------------------------------------ */
    unsigned char b[32];
    unsigned char a[32];
    unsigned char A[32];
    unsigned char B[32];

    /* SPK */
    cx_aes_key_t spk;
    unsigned char hmac_key[32];

    /* Tx key */
    unsigned char R[32];
    unsigned char r[32];

    /* prefix/mlsag hash */
    cx_sha3_t keccakF;
    cx_sha3_t keccakH;
    unsigned char prefixH[32];
    unsigned char mlsagH[32];
    unsigned char c[32];

    /* -- track tx-in/out and commitment -- */
    cx_sha256_t sha256_out_keys;
    unsigned char OUTK[32];

    cx_sha256_t sha256_commitment;
    unsigned char C[32];

    /* ------------------------------------------ */
    /* ---               UI/UX                --- */
    /* ------------------------------------------ */
    char ux_wallet_public_short_address[5 + 2 + 5 + 1];
    char ux_wallet_account_name[14];

    union {
        struct {
            char ux_info1[14];
            char ux_info2[14];
            /* menu */
            char ux_menu[16];
            // address to display: 95/106-chars + null
            char ux_address[160];
            // xmr to display: max pow(2,64) unit, aka 20-chars + '0' + dot + null
            char ux_amount[23];
            // addr mode
            unsigned char disp_addr_mode;
            // M.m address
            unsigned int disp_addr_M;
            unsigned int disp_addr_m;
            // payment id
            char payment_id[16];
        };
        struct {
            unsigned char tmp[340];
        };
    };
};
typedef struct incognito_v_state_s incognito_v_state_t;

#define SIZEOF_TX_VSTATE (sizeof(incognito_v_state_t) - OFFSETOF(incognito_v_state_t, state))

#define STATE_IDLE 0xC0
/* ---  ...  --- */
#define IO_OFFSET_END  (unsigned int)-1
#define IO_OFFSET_MARK (unsigned int)-2

#define EXPORT_VIEW_KEY 0xC001BEEF

#define DISP_MAIN       0x51
#define DISP_SUB        0x52
#define DISP_INTEGRATED 0x53

/* ---  IO constants  --- */
#define OFFSET_CLA 0
#define OFFSET_INS 1
#define OFFSET_P1 2
#define OFFSET_P2 3
#define OFFSET_P3 4
#define OFFSET_CDATA 5
#define OFFSET_EXT_CDATA 7

#define SW_OK 0x9000

#define SW_WRONG_LENGTH 0x6700

#define SW_SECURITY_PIN_LOCKED 0x6910
#define SW_SECURITY_LOAD_KEY 0x6911
#define SW_SECURITY_COMMITMENT_CONTROL 0x6912
#define SW_SECURITY_AMOUNT_CHAIN_CONTROL 0x6913
#define SW_SECURITY_COMMITMENT_CHAIN_CONTROL 0x6914
#define SW_SECURITY_OUTKEYS_CHAIN_CONTROL 0x6915
#define SW_SECURITY_MAXOUTPUT_REACHED 0x6916
#define SW_SECURITY_HMAC 0x6917
#define SW_SECURITY_RANGE_VALUE 0x6918
#define SW_SECURITY_INTERNAL 0x6919
#define SW_SECURITY_MAX_SIGNATURE_REACHED 0x691A
#define SW_SECURITY_PREFIX_HASH 0x691B
#define SW_SECURITY_LOCKED 0x69EE

#define SW_COMMAND_NOT_ALLOWED 0x6980
#define SW_SUBCOMMAND_NOT_ALLOWED 0x6981
#define SW_DENY 0x6982
#define SW_KEY_NOT_SET 0x6983
#define SW_WRONG_DATA 0x6984
#define SW_WRONG_DATA_RANGE 0x6985
#define SW_IO_FULL 0x6986

#define SW_CLIENT_NOT_SUPPORTED 0x6A30

#define SW_WRONG_P1P2 0x6b00
#define SW_INS_NOT_SUPPORTED 0x6d00
#define SW_PROTOCOL_NOT_SUPPORTED 0x6e00

#define SW_UNKNOWN 0x6f00

#endif


/* ---  Errors  --- */
#define ERROR(x) ((x) << 16)

#define ERROR_IO_OFFSET ERROR(1)
#define ERROR_IO_FULL   ERROR(2)

/* ---  INS  --- */

#define INS_NONE         0x00
#define INS_RESET        0x02
#define INS_LOCK_DISPLAY 0x04

#define INS_GET_KEY            0x20
#define INS_DISPLAY_ADDRESS    0x21