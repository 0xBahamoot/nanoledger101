#include "os.h"
#include "cx.h"
#include "incognito_types.h"
#include "incognito_api.h"
#include "incognito_vars.h"

#if defined(IODUMMYCRYPT)
#warning IODUMMYCRYPT activated
#endif
#if defined(IONOCRYPT)
#warning IONOCRYPT activated
#endif

/*
 * io_buff: contains current message part
 * io_off: offset in current message part
 * io_length: length of current message part
 */

 /* ----------------------------------------------------------------------- */
 /* MISC                                                                    */
 /* ----------------------------------------------------------------------- */
    void incognito_io_set_offset(unsigned int offset) {
    if (offset == IO_OFFSET_END) {
        G_incognito_vstate.io_offset = G_incognito_vstate.io_length;
    }
    else if (offset == IO_OFFSET_MARK) {
        G_incognito_vstate.io_offset = G_incognito_vstate.io_mark;
    }
    else if (offset < G_incognito_vstate.io_length) {
        G_incognito_vstate.io_offset = offset;
    }
    else {
        THROW(ERROR_IO_OFFSET);
    }
}

void incognito_io_mark() { G_incognito_vstate.io_mark = G_incognito_vstate.io_offset; }

void incognito_io_inserted(unsigned int len) {
    G_incognito_vstate.io_offset += len;
    G_incognito_vstate.io_length += len;
}

void incognito_io_discard(int clear) {
    G_incognito_vstate.io_length = 0;
    G_incognito_vstate.io_offset = 0;
    G_incognito_vstate.io_mark = 0;
    if (clear) {
        incognito_io_clear();
    }
}

void incognito_io_clear() { os_memset(G_incognito_vstate.io_buffer, 0, INCOGNITO_IO_BUFFER_LENGTH); }

/* ----------------------------------------------------------------------- */
/* INSERT data to be sent                                                  */
/* ----------------------------------------------------------------------- */

void incognito_io_hole(unsigned int sz) {
    if ((G_incognito_vstate.io_length + sz) > INCOGNITO_IO_BUFFER_LENGTH) {
        THROW(ERROR_IO_FULL);
    }
    os_memmove(G_incognito_vstate.io_buffer + G_incognito_vstate.io_offset + sz,
        G_incognito_vstate.io_buffer + G_incognito_vstate.io_offset,
        G_incognito_vstate.io_length - G_incognito_vstate.io_offset);
    G_incognito_vstate.io_length += sz;
}

void incognito_io_insert(unsigned char const* buff, unsigned int len) {
    incognito_io_hole(len);
    os_memmove(G_incognito_vstate.io_buffer + G_incognito_vstate.io_offset, buff, len);
    G_incognito_vstate.io_offset += len;
}

// void incognito_io_insert_hmac_for(unsigned char* buffer, int len, int type) {
//     // for now, only 32bytes block are allowed
//     if (len != 32) {
//         THROW(SW_WRONG_DATA);
//     }

//     unsigned char hmac[32 + 1 + 4];

//     os_memmove(hmac, buffer, 32);
//     hmac[32] = type;
//     if (type == TYPE_ALPHA) {
//         hmac[33] = (G_incognito_vstate.tx_sign_cnt >> 0) & 0xFF;
//         hmac[34] = (G_incognito_vstate.tx_sign_cnt >> 8) & 0xFF;
//         hmac[35] = (G_incognito_vstate.tx_sign_cnt >> 16) & 0xFF;
//         hmac[36] = (G_incognito_vstate.tx_sign_cnt >> 24) & 0xFF;
//     }
//     else {
//         hmac[33] = 0;
//         hmac[34] = 0;
//         hmac[35] = 0;
//         hmac[36] = 0;
//     }
//     cx_hmac_sha256(G_incognito_vstate.hmac_key, 32, hmac, 37, hmac, 32);
//     incognito_io_insert(hmac, 32);
// }

void incognito_io_insert_encrypt(unsigned char* buffer, int len, int type) {
    // for now, only 32bytes block are allowed
    if (len != 32) {
        THROW(SW_WRONG_DATA);
    }

    incognito_io_hole(len);

#if defined(IODUMMYCRYPT)
    for (int i = 0; i < len; i++) {
        G_incognito_vstate.io_buffer[G_incognito_vstate.io_offset + i] = buffer[i] ^ 0x55;
    }
#elif defined(IONOCRYPT)
    os_memmove(G_incognito_vstate.io_buffer + G_incognito_vstate.io_offset, buffer, len);
#else
    cx_aes(&G_incognito_vstate.spk, CX_ENCRYPT | CX_CHAIN_CBC | CX_LAST | CX_PAD_NONE, buffer, len,
        G_incognito_vstate.io_buffer + G_incognito_vstate.io_offset, len);
#endif
    G_incognito_vstate.io_offset += len;
    if (G_incognito_vstate.tx_in_progress) {
        incognito_io_insert_hmac_for(G_incognito_vstate.io_buffer + G_incognito_vstate.io_offset - len, len,
            type);
    }
}

void incognito_io_insert_u32(unsigned int v32) {
    incognito_io_hole(4);
    G_incognito_vstate.io_buffer[G_incognito_vstate.io_offset + 0] = v32 >> 24;
    G_incognito_vstate.io_buffer[G_incognito_vstate.io_offset + 1] = v32 >> 16;
    G_incognito_vstate.io_buffer[G_incognito_vstate.io_offset + 2] = v32 >> 8;
    G_incognito_vstate.io_buffer[G_incognito_vstate.io_offset + 3] = v32 >> 0;
    G_incognito_vstate.io_offset += 4;
}

void incognito_io_insert_u24(unsigned int v24) {
    incognito_io_hole(3);
    G_incognito_vstate.io_buffer[G_incognito_vstate.io_offset + 0] = v24 >> 16;
    G_incognito_vstate.io_buffer[G_incognito_vstate.io_offset + 1] = v24 >> 8;
    G_incognito_vstate.io_buffer[G_incognito_vstate.io_offset + 2] = v24 >> 0;
    G_incognito_vstate.io_offset += 3;
}

void incognito_io_insert_u16(unsigned int v16) {
    incognito_io_hole(2);
    G_incognito_vstate.io_buffer[G_incognito_vstate.io_offset + 0] = v16 >> 8;
    G_incognito_vstate.io_buffer[G_incognito_vstate.io_offset + 1] = v16 >> 0;
    G_incognito_vstate.io_offset += 2;
}

void incognito_io_insert_u8(unsigned int v8) {
    incognito_io_hole(1);
    G_incognito_vstate.io_buffer[G_incognito_vstate.io_offset + 0] = v8;
    G_incognito_vstate.io_offset += 1;
}

void incognito_io_insert_t(unsigned int T) {
    if (T & 0xFF00) {
        incognito_io_insert_u16(T);
    }
    else {
        incognito_io_insert_u8(T);
    }
}

void incognito_io_insert_tl(unsigned int T, unsigned int L) {
    incognito_io_insert_t(T);
    if (L < 128) {
        incognito_io_insert_u8(L);
    }
    else if (L < 256) {
        incognito_io_insert_u16(0x8100 | L);
    }
    else {
        incognito_io_insert_u8(0x82);
        incognito_io_insert_u16(L);
    }
}

void incognito_io_insert_tlv(unsigned int T, unsigned int L, unsigned char const* V) {
    incognito_io_insert_tl(T, L);
    incognito_io_insert(V, L);
}

/* ----------------------------------------------------------------------- */
/* FECTH data from received buffer                                         */
/* ----------------------------------------------------------------------- */
int incognito_io_fetch_available() { return G_incognito_vstate.io_length - G_incognito_vstate.io_offset; }
void incognito_io_assert_available(int sz) {
    if ((G_incognito_vstate.io_length - G_incognito_vstate.io_offset) < sz) {
        THROW(SW_WRONG_LENGTH + (sz & 0xFF));
    }
}

int incognito_io_fetch(unsigned char* buffer, int len) {
    incognito_io_assert_available(len);
    if (buffer) {
        os_memmove(buffer, G_incognito_vstate.io_buffer + G_incognito_vstate.io_offset, len);
    }
    G_incognito_vstate.io_offset += len;
    return len;
}

// static void incognito_io_verify_hmac_for(const unsigned char* buffer, int len,
//     unsigned char* expected_hmac, int type) {
//     // for now, only 32bytes block allowed
//     if (len != 32) {
//         THROW(SW_WRONG_DATA);
//     }

//     unsigned char hmac[37];
//     os_memmove(hmac, buffer, 32);
//     hmac[32] = type;
//     if (type == TYPE_ALPHA) {
//         hmac[33] = (G_incognito_vstate.tx_sign_cnt >> 0) & 0xFF;
//         hmac[34] = (G_incognito_vstate.tx_sign_cnt >> 8) & 0xFF;
//         hmac[35] = (G_incognito_vstate.tx_sign_cnt >> 16) & 0xFF;
//         hmac[36] = (G_incognito_vstate.tx_sign_cnt >> 24) & 0xFF;
//     }
//     else {
//         hmac[33] = 0;
//         hmac[34] = 0;
//         hmac[35] = 0;
//         hmac[36] = 0;
//     }
//     cx_hmac_sha256(G_incognito_vstate.hmac_key, 32, hmac, 37, hmac, 32);
//     if (os_memcmp(hmac, expected_hmac, 32)) {
//         incognito_lock_and_throw(SW_SECURITY_HMAC);
//     }
// }

// int incognito_io_fetch_decrypt(unsigned char* buffer, int len, int type) {
//     // for now, only 32bytes block allowed
//     if (len != 32) {
//         THROW(SW_WRONG_LENGTH);
//     }

//     if (G_incognito_vstate.tx_in_progress) {
//         incognito_io_assert_available(len + 32);
//         incognito_io_verify_hmac_for(G_incognito_vstate.io_buffer + G_incognito_vstate.io_offset, len,
//             G_incognito_vstate.io_buffer + G_incognito_vstate.io_offset + len,
//             type);
//     }
//     else {
//         incognito_io_assert_available(len);
//     }

//     if (buffer) {
// #if defined(IODUMMYCRYPT)
//         for (int i = 0; i < len; i++) {
//             buffer[i] = G_incognito_vstate.io_buffer[G_incognito_vstate.io_offset + i] ^ 0x55;
//         }
// #elif defined(IONOCRYPT)
//         os_memmove(buffer, G_incognito_vstate.io_buffer + G_incognito_vstate.io_offset, len);
// #else  // IOCRYPT
//         cx_aes(&G_incognito_vstate.spk, CX_DECRYPT | CX_CHAIN_CBC | CX_LAST | CX_PAD_NONE,
//             G_incognito_vstate.io_buffer + G_incognito_vstate.io_offset, len, buffer, len);
// #endif
//     }
//     G_incognito_vstate.io_offset += len;
//     if (G_incognito_vstate.tx_in_progress) {
//         G_incognito_vstate.io_offset += 32;
//     }
//     if (buffer) {
//         switch (type) {
//         case TYPE_SCALAR:
//             incognito_check_scalar_range_1N(buffer);
//             break;
//         case TYPE_AMOUNT_KEY:
//         case TYPE_DERIVATION:
//         case TYPE_ALPHA:
//             incognito_check_scalar_not_null(buffer);
//             break;
//         default:
//             THROW(SW_SECURITY_INTERNAL);
//         }
//     }
//     return len;
// }

// int incognito_io_fetch_decrypt_key(unsigned char* buffer) {
//     unsigned char* k;
//     incognito_io_assert_available(32);

//     k = G_incognito_vstate.io_buffer + G_incognito_vstate.io_offset;
//     // view?
//     if (os_memcmp(k, C_FAKE_SEC_VIEW_KEY, 32) == 0) {
//         G_incognito_vstate.io_offset += 32;
//         if (G_incognito_vstate.tx_in_progress) {
//             incognito_io_assert_available(32);
//             incognito_io_verify_hmac_for(C_FAKE_SEC_VIEW_KEY, 32,
//                 G_incognito_vstate.io_buffer + G_incognito_vstate.io_offset,
//                 TYPE_SCALAR);
//             G_incognito_vstate.io_offset += 32;
//         }
//         os_memmove(buffer, G_incognito_vstate.a, 32);
//         return 32;
//     }
//     // spend?
//     else if (os_memcmp(k, C_FAKE_SEC_SPEND_KEY, 32) == 0) {
//         switch (G_incognito_vstate.io_ins) {
//         case INS_VERIFY_KEY:
//         case INS_DERIVE_SECRET_KEY:
//             // case INS_GET_SUBADDRESS_SPEND_PUBLIC_KEY:
//             break;
//         default:
//             THROW(SW_WRONG_DATA);
//         }
//         G_incognito_vstate.io_offset += 32;
//         if (G_incognito_vstate.tx_in_progress) {
//             incognito_io_assert_available(32);
//             incognito_io_verify_hmac_for(C_FAKE_SEC_SPEND_KEY, 32,
//                 G_incognito_vstate.io_buffer + G_incognito_vstate.io_offset,
//                 TYPE_SCALAR);
//         }
//         os_memmove(buffer, G_incognito_vstate.b, 32);
//         return 32;
//     }
//     // else
//     else {
//         return incognito_io_fetch_decrypt(buffer, 32, TYPE_SCALAR);
//     }
// }

uint64_t incognito_io_fetch_varint() {
    uint64_t v64;
    G_incognito_vstate.io_offset +=
        incognito_decode_varint(G_incognito_vstate.io_buffer + G_incognito_vstate.io_offset,
            MIN(8, G_incognito_vstate.io_length - G_incognito_vstate.io_offset), &v64);
    return v64;
}

unsigned int incognito_io_fetch_u32() {
    unsigned int v32;
    incognito_io_assert_available(4);
    v32 = ((G_incognito_vstate.io_buffer[G_incognito_vstate.io_offset + 0] << 24) |
        (G_incognito_vstate.io_buffer[G_incognito_vstate.io_offset + 1] << 16) |
        (G_incognito_vstate.io_buffer[G_incognito_vstate.io_offset + 2] << 8) |
        (G_incognito_vstate.io_buffer[G_incognito_vstate.io_offset + 3] << 0));
    G_incognito_vstate.io_offset += 4;
    return v32;
}

unsigned int incognito_io_fetch_u24() {
    unsigned int v24;
    incognito_io_assert_available(3);
    v24 = ((G_incognito_vstate.io_buffer[G_incognito_vstate.io_offset + 0] << 16) |
        (G_incognito_vstate.io_buffer[G_incognito_vstate.io_offset + 1] << 8) |
        (G_incognito_vstate.io_buffer[G_incognito_vstate.io_offset + 2] << 0));
    G_incognito_vstate.io_offset += 3;
    return v24;
}

unsigned int incognito_io_fetch_u16() {
    unsigned int v16;
    incognito_io_assert_available(2);
    v16 = ((G_incognito_vstate.io_buffer[G_incognito_vstate.io_offset + 0] << 8) |
        (G_incognito_vstate.io_buffer[G_incognito_vstate.io_offset + 1] << 0));
    G_incognito_vstate.io_offset += 2;
    return v16;
}

unsigned int incognito_io_fetch_u8() {
    unsigned int v8;
    incognito_io_assert_available(1);
    v8 = G_incognito_vstate.io_buffer[G_incognito_vstate.io_offset];
    G_incognito_vstate.io_offset += 1;
    return v8;
}

int incognito_io_fetch_t(unsigned int* T) {
    *T = incognito_io_fetch_u8();
    if ((*T & 0x1F) == 0x1F) {
        *T = (*T << 8) | incognito_io_fetch_u8();
    }
    return 0;
}

int incognito_io_fetch_l(unsigned int* L) {
    *L = incognito_io_fetch_u8();

    if ((*L & 0x80) != 0) {
        *L &= 0x7F;
        if (*L == 1) {
            *L = incognito_io_fetch_u8();
        }
        else if (*L == 2) {
            *L = incognito_io_fetch_u16();
        }
        else {
            *L = -1;
        }
    }
    return 0;
}

int incognito_io_fetch_tl(unsigned int* T, unsigned int* L) {
    incognito_io_fetch_t(T);
    incognito_io_fetch_l(L);
    return 0;
}

int incognito_io_fetch_nv(unsigned char* buffer, int len) {
    incognito_io_assert_available(len);
    incognito_nvm_write(buffer, G_incognito_vstate.io_buffer + G_incognito_vstate.io_offset, len);
    G_incognito_vstate.io_offset += len;
    return len;
}

/* ----------------------------------------------------------------------- */
/* REAL IO                                                                 */
/* ----------------------------------------------------------------------- */

#define MAX_OUT INCOGNITO_APDU_LENGTH

int incognito_io_do(unsigned int io_flags) {
    // if IO_ASYNCH_REPLY has been  set,
    //  incognito_io_exchange will return when  IO_RETURN_AFTER_TX will set in ui
    if (io_flags & IO_ASYNCH_REPLY) {
        incognito_io_exchange(CHANNEL_APDU | IO_ASYNCH_REPLY, 0);
    }
    // else send data now
    else {
        G_incognito_vstate.io_offset = 0;
        if (G_incognito_vstate.io_length > MAX_OUT) {
            THROW(SW_IO_FULL);
        }
        os_memmove(G_io_apdu_buffer, G_incognito_vstate.io_buffer + G_incognito_vstate.io_offset,
            G_incognito_vstate.io_length);

        if (io_flags & IO_RETURN_AFTER_TX) {
            incognito_io_exchange(CHANNEL_APDU | IO_RETURN_AFTER_TX, G_incognito_vstate.io_length);
            return 0;
        }
        else {
            incognito_io_exchange(CHANNEL_APDU, G_incognito_vstate.io_length);
        }
    }

    //--- set up received data  ---
    G_incognito_vstate.io_offset = 0;
    G_incognito_vstate.io_length = 0;
    G_incognito_vstate.io_protocol_version = G_io_apdu_buffer[0];
    G_incognito_vstate.io_ins = G_io_apdu_buffer[1];
    G_incognito_vstate.io_p1 = G_io_apdu_buffer[2];
    G_incognito_vstate.io_p2 = G_io_apdu_buffer[3];
    G_incognito_vstate.io_lc = 0;
    G_incognito_vstate.io_le = 0;
    G_incognito_vstate.io_lc = G_io_apdu_buffer[4];
    os_memmove(G_incognito_vstate.io_buffer, G_io_apdu_buffer + 5, G_incognito_vstate.io_lc);
    G_incognito_vstate.io_length = G_incognito_vstate.io_lc;

    return 0;
}
