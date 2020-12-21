#include "utils.h"
#include "globals.h"
#include "crypto.h"


static const uint32_t HARDENED_OFFSET = 0x80000000;

static const uint32_t derivePath[BIP32_PATH] = {
  44 | HARDENED_OFFSET,
  587 | HARDENED_OFFSET,
  0 | HARDENED_OFFSET,
  0 | HARDENED_OFFSET,
  0 | HARDENED_OFFSET
};


void incognito_init_private_key() {
    uint32_t bip32Path[BIP32_PATH];
    unsigned char seed[32];
    unsigned char chain[32];

    os_memmove(bip32Path, derivePath, sizeof(derivePath));
    bip32Path[2] = 0 | HARDENED_OFFSET;
    os_perso_derive_node_bip32(CX_CURVE_SECP256K1, bip32Path, 5, seed, chain);

    incognito_keccak_F(seed, 32, G_crypto_state_t.b);
    incognito_reduce(G_crypto_state_t.b, G_crypto_state_t.b);
    incognito_keccak_F(G_crypto_state_t.b, 32, G_crypto_state_t.a);
    incognito_reduce(G_crypto_state_t.a, G_crypto_state_t.a);

    incognito_ecmul_G(G_crypto_state_t.A, G_crypto_state_t.a);
    incognito_ecmul_G(G_crypto_state_t.B, G_crypto_state_t.b);

    // generate key protection
    incognito_aes_derive(&G_crypto_state_t.spk, chain, G_crypto_state_t.a, G_crypto_state_t.b);
}

void incognito_reset_crypto_state() {

}