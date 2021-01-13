#ifndef _CRYPTO_H_
#define _CRYPTO_H_

/* ----------------------------------------------------------------------- */
/* ---                              CRYPTO                            ---- */
/* ----------------------------------------------------------------------- */
extern const unsigned char C_ED25519_ORDER[];

void incognito_aes_derive(cx_aes_key_t *sk, unsigned char *seed32, unsigned char *a, unsigned char *b);
void incognito_aes_generate(cx_aes_key_t *sk);

void incognito_hash_init_keccak(cx_hash_t *hasher);
void incognito_hash_init_sha256(cx_hash_t *hasher);
void incognito_hash_update(cx_hash_t *hasher, unsigned char *buf, unsigned int len);
int incognito_hash_final(cx_hash_t *hasher, unsigned char *out);
int incognito_hash(unsigned int algo, cx_hash_t *hasher, unsigned char *buf, unsigned int len,
                   unsigned char *out);

#define incognito_sha3_init() incognito_hash_init_sha3((cx_hash_t *)&G_crypto_state_t.keccakF)
#define incognito_sha3_update(buf, len) \
    incognito_hash_update((cx_hash_t *)&G_crypto_state_t.keccakF, (buf), (len))
#define incognito_sha3_final(out) incognito_hash_final((cx_hash_t *)&G_crypto_state_t.keccakF, (out))
#define incognito_sha3(buf, len, out) \
    incognito_hash(CX_SHA3, (cx_hash_t *)&G_crypto_state_t.keccakF, (buf), (len), (out))

#define incognito_keccak_init_F() incognito_hash_init_keccak((cx_hash_t *)&G_crypto_state_t.keccakF)
#define incognito_keccak_update_F(buf, len) \
    incognito_hash_update((cx_hash_t *)&G_crypto_state_t.keccakF, (buf), (len))
#define incognito_keccak_final_F(out) incognito_hash_final((cx_hash_t *)&G_crypto_state_t.keccakF, (out))
#define incognito_keccak_F(buf, len, out) \
    incognito_hash(CX_KECCAK, (cx_hash_t *)&G_crypto_state_t.keccakF, (buf), (len), (out))

#define incognito_keccak_init_H() incognito_hash_init_keccak((cx_hash_t *)&G_crypto_state_t.keccakH)
#define incognito_keccak_update_H(buf, len) \
    incognito_hash_update((cx_hash_t *)&G_crypto_state_t.keccakH, (buf), (len))
#define incognito_keccak_final_H(out) incognito_hash_final((cx_hash_t *)&G_crypto_state_t.keccakH, (out))
#define incognito_keccak_H(buf, len, out) \
    incognito_hash(CX_KECCAK, (cx_hash_t *)&G_crypto_state_t.keccakH, (buf), (len), (out))

#define incognito_sha256_commitment_init() \
    incognito_hash_init_sha256((cx_hash_t *)&G_crypto_state_t.sha256_commitment)
#define incognito_sha256_commitment_update(buf, len) \
    incognito_hash_update((cx_hash_t *)&G_crypto_state_t.sha256_commitment, (buf), (len))
#define incognito_sha256_commitment_final(out)                             \
    incognito_hash_final((cx_hash_t *)&G_crypto_state_t.sha256_commitment, \
                         (out) ? (out) : G_crypto_state_t.C)

#define incognito_sha256_outkeys_init() \
    incognito_hash_init_sha256((cx_hash_t *)&G_crypto_state_t.sha256_out_keys)
#define incognito_sha256_outkeys_update(buf, len) \
    incognito_hash_update((cx_hash_t *)&G_crypto_state_t.sha256_out_keys, (buf), (len))
#define incognito_sha256_outkeys_final(out) \
    incognito_hash_final((cx_hash_t *)&G_crypto_state_t.sha256_out_keys, (out))

/*
 *  check 1<s<N, else throw
 */
void incognito_check_scalar_range_1N(unsigned char *s);

/*
 *  check 1<s, else throw
 */
void incognito_check_scalar_not_null(unsigned char *s);

/**
 * LE-7-bits encoding. High bit set says one more byte to decode.
 */
unsigned int incognito_encode_varint(unsigned char *varint, unsigned int max_len, uint64_t v);

/**
 * LE-7-bits decoding. High bit set says one more byte to decode.
 */
unsigned int incognito_decode_varint(unsigned char *varint, unsigned int max_len, uint64_t *v);

/** */
void incognito_reverse32(unsigned char *rscal, unsigned char *scal);

/**
 * Hps: keccak(drv_data|varint(out_idx))
 */
void incognito_derivation_to_scalar(unsigned char *scalar, unsigned char *drv_data,
                                    unsigned int out_idx);

/*
 * W = k.P
 */
void incognito_ecmul_k(unsigned char *W, unsigned char *P, unsigned char *scalar32);
/*
 * W = 8k.P
 */
void incognito_ecmul_8k(unsigned char *W, unsigned char *P, unsigned char *scalar32);

/*
 * W = 8.P
 */
void incognito_ecmul_8(unsigned char *W, unsigned char *P);

/*
 * W = k.G
 */
void incognito_ecmul_G(unsigned char *W, unsigned char *scalar32);

/*
 * W = k.H
 */
void incognito_ecmul_H(unsigned char *W, unsigned char *scalar32);

/**
 *  keccak("amount"|sk)
 */
void incognito_ecdhHash(unsigned char *x, unsigned char *k);

/**
 * keccak("commitment_mask"|sk) %order
 */
void incognito_genCommitmentMask(unsigned char *c, unsigned char *sk);

/*
 * W = P+Q
 */
void incognito_ecadd(unsigned char *W, unsigned char *P, unsigned char *Q);
/*
 * W = P-Q
 */
void incognito_ecsub(unsigned char *W, unsigned char *P, unsigned char *Q);

/* r = (a+b) %order */
void incognito_addm(unsigned char *r, unsigned char *a, unsigned char *b);

/* r = (a-b) %order */
void incognito_subm(unsigned char *r, unsigned char *a, unsigned char *b);

/* r = (a*b) %order */
void incognito_multm(unsigned char *r, unsigned char *a, unsigned char *b);

/* r = (a*8) %order */
void incognito_multm_8(unsigned char *r, unsigned char *a);

/* */
void incognito_reduce(unsigned char *r, unsigned char *a);

void incognito_rng_mod_order(unsigned char *r);

void incognito_get_subaddress_secret_key(unsigned char *sub_s, unsigned char *s, unsigned char *index);
void incognito_doublesha256(unsigned char *buf, unsigned int len, unsigned char *out);
void incognito_add_B58checksum(unsigned char *preEncode, unsigned int len, unsigned char *buf);
void incognito_hash_to_scalar(unsigned char *scalar, unsigned char *raw, unsigned int raw_len);
void incognito_hash_to_point(unsigned char *ec, unsigned char *ec_pub);
void incognito_hash_to_ec(unsigned char *ec, unsigned char *ec_pub);
void incognito_generate_key_image(unsigned char *img, unsigned char *P, unsigned char *x);
#endif