#ifndef INCOGNITO_API_H
#define INCOGNITO_API_H

void incognito_install();
void incognito_init(void);
void incognito_init_private_key(void);
void incognito_wipe_private_key(void);


void incognito_init_ux(void);
int incognito_handler(void);

int incognito_apdu_reset(void);
int incognito_apdu_lock(void);
void incognito_lock_and_throw(int sw);
int incognito_apdu_get_subaddress_secret_key(void);
void incognito_get_subaddress_secret_key(unsigned char *sub_s, unsigned char *s, unsigned char *index);


/* ----------------------------------------------------------------------- */
/* ---                                IO                              ---- */
/* ----------------------------------------------------------------------- */

void incognito_io_discard(int clear);
void incognito_io_clear(void);
void incognito_io_set_offset(unsigned int offset);
void incognito_io_mark(void);
void incognito_io_rewind(void);
void incognito_io_hole(unsigned int sz);
void incognito_io_inserted(unsigned int len);
void incognito_io_insert(unsigned char const *buffer, unsigned int len);
void incognito_io_insert_encrypt(unsigned char *buffer, int len, int type);
void incognito_io_insert_hmac_for(unsigned char *buffer, int len, int type);

void incognito_io_insert_u32(unsigned int v32);
void incognito_io_insert_u24(unsigned int v24);
void incognito_io_insert_u16(unsigned int v16);
void incognito_io_insert_u8(unsigned int v8);
void incognito_io_insert_t(unsigned int T);
void incognito_io_insert_tl(unsigned int T, unsigned int L);
void incognito_io_insert_tlv(unsigned int T, unsigned int L, unsigned char const *V);

int incognito_io_fetch_available();
void incognito_io_fetch_buffer(unsigned char *buffer, unsigned int len);
uint64_t incognito_io_fetch_varint(void);
unsigned int incognito_io_fetch_u32(void);
unsigned int incognito_io_fetch_u24(void);
unsigned int incognito_io_fetch_u16(void);
unsigned int incognito_io_fetch_u8(void);
int incognito_io_fetch_t(unsigned int *T);
int incognito_io_fetch_l(unsigned int *L);
int incognito_io_fetch_tl(unsigned int *T, unsigned int *L);
int incognito_io_fetch_nv(unsigned char *buffer, int len);
int incognito_io_fetch(unsigned char *buffer, int len);
int incognito_io_fetch_decrypt(unsigned char *buffer, int len, int type);
int incognito_io_fetch_decrypt_key(unsigned char *buffer);

int incognito_io_do(unsigned int io_flags);


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

#define incognito_keccak_init_F() incognito_hash_init_keccak((cx_hash_t *)&G_incognito_vstate.keccakF)
#define incognito_keccak_update_F(buf, len) \
    incognito_hash_update((cx_hash_t *)&G_incognito_vstate.keccakF, (buf), (len))
#define incognito_keccak_final_F(out) incognito_hash_final((cx_hash_t *)&G_incognito_vstate.keccakF, (out))
#define incognito_keccak_F(buf, len, out) \
    incognito_hash(CX_KECCAK, (cx_hash_t *)&G_incognito_vstate.keccakF, (buf), (len), (out))

#define incognito_keccak_init_H() incognito_hash_init_keccak((cx_hash_t *)&G_incognito_vstate.keccakH)
#define incognito_keccak_update_H(buf, len) \
    incognito_hash_update((cx_hash_t *)&G_incognito_vstate.keccakH, (buf), (len))
#define incognito_keccak_final_H(out) incognito_hash_final((cx_hash_t *)&G_incognito_vstate.keccakH, (out))
#define incognito_keccak_H(buf, len, out) \
    incognito_hash(CX_KECCAK, (cx_hash_t *)&G_incognito_vstate.keccakH, (buf), (len), (out))

#define incognito_sha256_commitment_init() \
    incognito_hash_init_sha256((cx_hash_t *)&G_incognito_vstate.sha256_commitment)
#define incognito_sha256_commitment_update(buf, len) \
    incognito_hash_update((cx_hash_t *)&G_incognito_vstate.sha256_commitment, (buf), (len))
#define incognito_sha256_commitment_final(out)                            \
    incognito_hash_final((cx_hash_t *)&G_incognito_vstate.sha256_commitment, \
                      (out) ? (out) : G_incognito_vstate.C)

#define incognito_sha256_outkeys_init() \
    incognito_hash_init_sha256((cx_hash_t *)&G_incognito_vstate.sha256_out_keys)
#define incognito_sha256_outkeys_update(buf, len) \
    incognito_hash_update((cx_hash_t *)&G_incognito_vstate.sha256_out_keys, (buf), (len))
#define incognito_sha256_outkeys_final(out) \
    incognito_hash_final((cx_hash_t *)&G_incognito_vstate.sha256_out_keys, (out))

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


int incognito_base58_public_key(char *str_b58, unsigned char *view, unsigned char *spend,
                             unsigned char is_subbadress, unsigned char *paymanetID);

#endif

#define incognito_nvm_write   nvm_write
#define incognito_io_exchange io_exchange