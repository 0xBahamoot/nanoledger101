#include "utils.h"
#include "globals.h"
#include "crypto.h"
#include "key.h"
#include "cx.h"

static const uint32_t HARDENED_OFFSET = 0x80000000;

static const uint32_t derivePath[BIP32_PATH] = {
  44 | HARDENED_OFFSET,
  587 | HARDENED_OFFSET,
  0 | HARDENED_OFFSET,
  0 | HARDENED_OFFSET,
  0 | HARDENED_OFFSET
  };



void incognito_gen_private_key(uint32_t account_number, privatekey_t* privKey) {
  uint32_t bip32Path[BIP32_PATH];
  unsigned char seed[32];

  os_memmove(bip32Path, derivePath, sizeof(derivePath));
  bip32Path[2] = account_number | HARDENED_OFFSET;
  os_perso_derive_node_bip32(CX_CURVE_SECP256K1, bip32Path, 5, seed, privKey->chain_code);

  incognito_keccak_F(seed, 32, privKey->key);
  incognito_reduce(privKey->key, privKey->key);
  privKey->depth = 0;
  privKey->child_number = account_number;

  unsigned char child_number[4];
  child_number[0] = (G_crypto_state_t.key.child_number >> 24) & 0xFF;
  child_number[1] = (G_crypto_state_t.key.child_number >> 16) & 0xFF;
  child_number[2] = (G_crypto_state_t.key.child_number >> 8) & 0xFF;
  child_number[3] = G_crypto_state_t.key.child_number & 0xFF;
  uint8_t buffer[64];
  cx_hmac_sha512(child_number, 4, privKey->chain_code, 32, buffer, CX_SHA512_SIZE);

  os_memmove(G_crypto_state_t.key.chain_code, buffer + 32, 32);
  }

void incognito_init_private_key() {
  incognito_gen_private_key(2, &G_crypto_state_t.key);

  incognito_keccak_F(G_crypto_state_t.key.key, 32, G_crypto_state_t.a);
  incognito_reduce(G_crypto_state_t.a, G_crypto_state_t.a);

  incognito_ecmul_G(G_crypto_state_t.A, G_crypto_state_t.a);
  incognito_ecmul_G(G_crypto_state_t.B, G_crypto_state_t.key.key);

  // generate key protection
  incognito_aes_derive(&G_crypto_state_t.spk, G_crypto_state_t.key.chain_code, G_crypto_state_t.a, G_crypto_state_t.key.key);

  }

void incognito_load_key(unsigned char* key[69], privatekey_t* privKey) {

  }

void incognito_reset_crypto_state() {

  }

void incognito_export_private_key() {

  }
