#include "os.h"
#include "cx.h"
#include "incognito_types.h"
#include "incognito_api.h"
#include "incognito_vars.h"
#include <string.h> 
/* ----------------------------------------------------------------------- */
/* --- Reset                                                           --- */
/* ----------------------------------------------------------------------- */
#define INCOGNITO_SUPPORTED_CLIENT_SIZE 2
const char* const incognito_supported_client[INCOGNITO_SUPPORTED_CLIENT_SIZE] = { "0.17.0.", "0.17.1." };

int incognito_apdu_reset() {
    unsigned int client_version_len;
    char client_version[16];
    client_version_len = G_incognito_vstate.io_length - G_incognito_vstate.io_offset;
    if (client_version_len > 14) {
        THROW(SW_CLIENT_NOT_SUPPORTED + 1);
    }
    incognito_io_fetch((unsigned char*)&client_version[0], client_version_len);
    client_version[client_version_len] = '.';
    client_version_len++;
    client_version[client_version_len] = 0;
    unsigned int i = 0;
    while (i < INCOGNITO_SUPPORTED_CLIENT_SIZE) {
        unsigned int incognito_supported_client_len = strlen((char*)PIC(incognito_supported_client[i]));
        if ((incognito_supported_client_len <= client_version_len) &&
            (os_memcmp((char*)PIC(incognito_supported_client[i]), client_version,
                incognito_supported_client_len) == 0)) {
            break;
        }
        i++;
    }
    if (i == INCOGNITO_SUPPORTED_CLIENT_SIZE) {
        THROW(SW_CLIENT_NOT_SUPPORTED);
    }

    incognito_io_discard(0);
    incognito_init();
    incognito_io_insert_u8(APPVERSION[0]);
    incognito_io_insert_u8(APPVERSION[2]);
    incognito_io_insert_u8(APPVERSION[4]);
    return SW_OK;
}
/* ----------------------------------------------------------------------- */
/* --- LOCK                                                           --- */
/* ----------------------------------------------------------------------- */
int incognito_apdu_lock() {
    incognito_io_discard(0);
    incognito_lock_and_throw(SW_SECURITY_LOCKED);
    return SW_SECURITY_LOCKED;
}

void incognito_lock_and_throw(int sw) {
    G_incognito_vstate.protocol_barrier = PROTOCOL_LOCKED;
    snprintf(G_incognito_vstate.ux_info1, sizeof(G_incognito_vstate.ux_info1), "Security Err");
    snprintf(G_incognito_vstate.ux_info2, sizeof(G_incognito_vstate.ux_info2), "%x", sw);
    // ui_menu_info_display(0);
    THROW(sw);
}



/* ----------------------------------------------------------------------- */
/* ---  Install/ReInstall Incognito app                                   --- */
/* ----------------------------------------------------------------------- */

const unsigned char C_MAGIC[8] = { 'I', 'N', 'C', 'O', 'G', 'N', 'I', 'T' };

void incognito_install() {
    unsigned char c;

    // full reset data
    incognito_nvm_write((void*)N_incognito_pstate, NULL, sizeof(incognito_nv_state_t));

    // set mode key
    c = KEY_MODE_SEED;
    nvm_write((void*)&N_incognito_pstate->key_mode, &c, 1);

    // set net id
    // incognito_nvm_write((void*)&N_incognito_pstate->network_id, &netId, 1);

    // write magic
    incognito_nvm_write((void*)N_incognito_pstate->magic, (void*)C_MAGIC, sizeof(C_MAGIC));
}


/* ----------------------------------------------------------------------- */
/* --- Boot                                                            --- */
/* ----------------------------------------------------------------------- */
void incognito_init() {
    os_memset(&G_incognito_vstate, 0, sizeof(incognito_v_state_t));

    // first init ?
    if (os_memcmp((void*)N_incognito_pstate->magic, (void*)C_MAGIC, sizeof(C_MAGIC)) != 0) {
        incognito_install();
    }

    G_incognito_vstate.protocol = 0xff;
    G_incognito_vstate.protocol_barrier = PROTOCOL_UNLOCKED;

    // load key
    incognito_init_private_key();
    // ux conf
    incognito_init_ux();
    // Let's go!
    G_incognito_vstate.state = STATE_IDLE;
}

/* ----------------------------------------------------------------------- */
/* --- init private keys                                               --- */
/* ----------------------------------------------------------------------- */
void incognito_wipe_private_key() {
    os_memset(G_incognito_vstate.a, 0, 32);
    os_memset(G_incognito_vstate.b, 0, 32);
    os_memset(G_incognito_vstate.A, 0, 32);
    os_memset(G_incognito_vstate.B, 0, 32);
    os_memset(&G_incognito_vstate.spk, 0, sizeof(G_incognito_vstate.spk));
    G_incognito_vstate.key_set = 0;
}

void incognito_init_private_key() {
    unsigned int path[5];
    unsigned char seed[32];
    unsigned char chain[32];

    // generate account keys

    // m / purpose' / coin_type' / account' / change / address_index
    // m / 44'      / 128'       / 0'       / 0      / 0
    path[0] = 0x8000002C;
    path[1] = 0x8000024B;
    path[2] = 0x80000000 | N_incognito_pstate->account_id;
    path[3] = 0x00000000;
    path[4] = 0x00000000;
    os_perso_derive_node_bip32(CX_CURVE_SECP256K1, path, 5, seed, chain);

    // switch (N_incognito_pstate->key_mode) {
    // case KEY_MODE_SEED:

    incognito_keccak_F(seed, 32, G_incognito_vstate.b);
    incognito_reduce(G_incognito_vstate.b, G_incognito_vstate.b);
    incognito_keccak_F(G_incognito_vstate.b, 32, G_incognito_vstate.a);
    incognito_reduce(G_incognito_vstate.a, G_incognito_vstate.a);
    //     break;

    // case KEY_MODE_EXTERNAL:
    //     os_memmove(G_incognito_vstate.a, (void*)N_incognito_pstate->a, 32);
    //     os_memmove(G_incognito_vstate.b, (void*)N_incognito_pstate->b, 32);
    //     break;

    // default:
    //     THROW(SW_SECURITY_LOAD_KEY);
    //     return;
    // }
    incognito_ecmul_G(G_incognito_vstate.A, G_incognito_vstate.a);
    incognito_ecmul_G(G_incognito_vstate.B, G_incognito_vstate.b);

    // generate key protection
    incognito_aes_derive(&G_incognito_vstate.spk, chain, G_incognito_vstate.a, G_incognito_vstate.b);

    G_incognito_vstate.key_set = 1;
}



/* ----------------------------------------------------------------------- */
/* ---  Set up ui/ux                                                   --- */
/* ----------------------------------------------------------------------- */
void incognito_init_ux() {
    //     incognito_base58_public_key(G_incognito_vstate.ux_address, G_incognito_vstate.A, G_incognito_vstate.B, 0,
    //                              NULL);
    //     os_memset(G_incognito_vstate.ux_wallet_public_short_address, '.',
    //               sizeof(G_incognito_vstate.ux_wallet_public_short_address));

#ifdef HAVE_UX_FLOW

#ifdef UI_NANO_X
    snprintf(G_incognito_vstate.ux_wallet_account_name, sizeof(G_incognito_vstate.ux_wallet_account_name),
        "XMR / %d", N_incognito_pstate->account_id);
    os_memmove(G_incognito_vstate.ux_wallet_public_short_address, G_incognito_vstate.ux_address, 5);
    os_memmove(G_incognito_vstate.ux_wallet_public_short_address + 7,
        G_incognito_vstate.ux_address + 95 - 5, 5);
    G_incognito_vstate.ux_wallet_public_short_address[12] = 0;
#else
    snprintf(G_incognito_vstate.ux_wallet_account_name, sizeof(G_incognito_vstate.ux_wallet_account_name),
        "     XMR / %d", N_incognito_pstate->account_id);
    os_memmove(G_incognito_vstate.ux_wallet_public_short_address, G_incognito_vstate.ux_address, 4);
    os_memmove(G_incognito_vstate.ux_wallet_public_short_address + 6,
        G_incognito_vstate.ux_address + 95 - 4, 4);
    G_incognito_vstate.ux_wallet_public_short_address[10] = 0;
#endif

#else

    snprintf(G_incognito_vstate.ux_wallet_account_name, sizeof(G_incognito_vstate.ux_wallet_account_name),
        "XMR / %d", N_incognito_pstate->account_id);
    os_memmove(G_incognito_vstate.ux_wallet_public_short_address, G_incognito_vstate.ux_address, 5);
    os_memmove(G_incognito_vstate.ux_wallet_public_short_address + 7,
        G_incognito_vstate.ux_address + 95 - 5, 5);
    G_incognito_vstate.ux_wallet_public_short_address[12] = 0;

#endif
}