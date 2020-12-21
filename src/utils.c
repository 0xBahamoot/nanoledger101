#include "os.h"
#include "cx.h"
#include <stdbool.h>
#include <stdlib.h>
#include "utils.h"
#include "menu.h"
#include "crypto.h"
#include "globals.h"

#define ACCOUNT_ADDRESS_PREFIX 1

static const char BASE_58_ALPHABET[] = { '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
                                        'H', 'J', 'K', 'L', 'M', 'N', 'P', 'Q',
                                        'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g',
                                        'h', 'i', 'j', 'k', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                        'w', 'x', 'y', 'z' };

unsigned char encodeBase58(unsigned char WIDE* in, unsigned char length,
    unsigned char* out, unsigned char maxoutlen) {
    unsigned char tmp[164];
    unsigned char buffer[164];
    unsigned char j;
    unsigned char startAt;
    unsigned char zeroCount = 0;
    if (length > sizeof(tmp)) {
        THROW(INVALID_PARAMETER);
    }
    os_memmove(tmp, in, length);
    while ((zeroCount < length) && (tmp[zeroCount] == 0)) {
        ++zeroCount;
    }
    j = 2 * length;
    startAt = zeroCount;
    while (startAt < length) {
        unsigned short remainder = 0;
        unsigned char divLoop;
        for (divLoop = startAt; divLoop < length; divLoop++) {
            unsigned short digit256 = (unsigned short)(tmp[divLoop] & 0xff);
            unsigned short tmpDiv = remainder * 256 + digit256;
            tmp[divLoop] = (unsigned char)(tmpDiv / 58);
            remainder = (tmpDiv % 58);
        }
        if (tmp[startAt] == 0) {
            ++startAt;
        }
        buffer[--j] = (unsigned char)BASE_58_ALPHABET[remainder];
    }
    while ((j < (2 * length)) && (buffer[j] == BASE_58_ALPHABET[0])) {
        ++j;
    }
    while (zeroCount-- > 0) {
        buffer[--j] = BASE_58_ALPHABET[0];
    }
    length = 2 * length - j;
    if (maxoutlen < length) {
        THROW(EXCEPTION_OVERFLOW);
    }
    os_memmove(out, (buffer + j), length);
    return length;
}

void getAddressStringFromBinary(uint8_t* publicKey, char* address) {
    uint8_t buffer[36];
    uint8_t hashAddress[32];

    os_memmove(buffer, publicKey, 32);
    cx_hash_sha256(buffer, 32, hashAddress, 32);
    cx_hash_sha256(hashAddress, 32, hashAddress, 32);
    os_memmove(buffer + 32, hashAddress, 4);

    snprintf(address, sizeof(address), "lol");
    address[encodeBase58(buffer, 36, (unsigned char*)address + 3, 51) + 3] = '\0';
}

int getPublicKey(uint32_t accountNumber, uint8_t* publicKeyArray, unsigned char* view, unsigned char* spend) {
    // unsigned char data[72 + 8];
    // unsigned int offset;
    // unsigned int prefix;
    // prefix = 53;

    // offset = incognito_encode_varint(data, 8, prefix);

    // os_memmove(data + offset, spend, 32);
    // os_memmove(data + offset + 32, view, 32);
    // offset += 64;
    // incognito_keccak_F(data, offset, G_crypto_state_t.mlsagH);
    // os_memmove(data + offset, G_crypto_state_t.mlsagH, 4);
    // offset += 4;
    // char* str_b58;
    // str_b58 = (char*)G_io_state_t.io_buffer;
    // unsigned int full_block_count = (offset) / FULL_BLOCK_SIZE;
    // unsigned int last_block_size = (offset) % FULL_BLOCK_SIZE;
    // PRINTF("BIP32: %.*H\n", full_block_count, last_block_size, FULL_BLOCK_SIZE);
    // for (size_t i = 0; i < full_block_count; ++i) {
    //     encode_block(data + i * FULL_BLOCK_SIZE, FULL_BLOCK_SIZE, &str_b58[i * FULL_ENCODED_BLOCK_SIZE]);
    // }

    // if (0 < last_block_size) {
    //     encode_block(data + full_block_count * FULL_BLOCK_SIZE, last_block_size, &str_b58[full_block_count * FULL_ENCODED_BLOCK_SIZE]);
    // }

    // str_b58[ADDR_LEN] = '\0';
    // PRINTF("publicKey: %s\n", str_b58);
    return 0;
}

uint32_t readUint32BE(uint8_t* buffer) {
    return (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | (buffer[3]);
}

void getPrivateKey(uint32_t accountNumber, cx_ecfp_private_key_t* privateKey) {
    // uint8_t privateKeyData[32];

    // os_memset(privateKeyData, 0, sizeof(privateKeyData));
}

void sendResponse(uint8_t tx, bool approve) {
    G_io_apdu_buffer[tx++] = approve ? 0x90 : 0x69;
    G_io_apdu_buffer[tx++] = approve ? 0x00 : 0x85;
    // Send back the response, do not restart the event loop
    io_exchange(CHANNEL_APDU | IO_RETURN_AFTER_TX, tx);
    // Display back the original UX
    ui_idle();
}

unsigned int ui_prepro(const bagl_element_t* element) {
    unsigned int display = 1;
    if (element->component.userid > 0) {
        display = (ux_step == element->component.userid - 1);
        if (display) {
            if (element->component.userid == 1) {
                UX_CALLBACK_SET_INTERVAL(2000);
            }
            else {
                UX_CALLBACK_SET_INTERVAL(MAX(3000, 1000 + bagl_label_roundtrip_duration_ms(element, 7)));
            }
        }
    }
    return display;
}


// /* ----------------------------------------------------------------------- */
// /* INSERT data to be sent                                                  */
// /* ----------------------------------------------------------------------- */

// void incognito_io_hole(unsigned int sz) {
//     if ((G_io_state_t.io_length + sz) > INCOGNITO_IO_BUFFER_LENGTH) {
//         THROW(ERROR_IO_FULL);
//     }
//     os_memmove(G_io_state_t.io_buffer + G_io_state_t.io_offset + sz,
//         G_io_state_t.io_buffer + G_io_state_t.io_offset,
//         G_io_state_t.io_length - G_io_state_t.io_offset);
//     G_io_state_t.io_length += sz;
// }

// void incognito_io_insert(unsigned char const* buff, unsigned int len) {
//     incognito_io_hole(len);
//     os_memmove(G_io_state_t.io_buffer + G_io_state_t.io_offset, buff, len);
//     G_io_state_t.io_offset += len;
// }


const char alphabet[] = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
#define alphabet_size (sizeof(alphabet) - 1)
const unsigned int encoded_block_sizes[] = { 0, 2, 3, 5, 6, 7, 9, 10, 11 };
#define FULL_BLOCK_SIZE         8  //(sizeof(encoded_block_sizes) / sizeof(encoded_block_sizes[0]) - 1)
#define FULL_ENCODED_BLOCK_SIZE 11  // encoded_block_sizes[full_block_size];
#define ADDR_CHECKSUM_SIZE      4
#define ADDR_LEN                95
#define INTEGRATED_ADDR_LEN     106

static uint64_t uint_8be_to_64(const unsigned char* data, size_t size) {
    uint64_t res = 0;
    switch (9 - size) {
    case 1:
        res |= *data++;
    case 2:
        res <<= 8;
        res |= *data++;
    case 3:
        res <<= 8;
        res |= *data++;
    case 4:
        res <<= 8;
        res |= *data++;
    case 5:
        res <<= 8;
        res |= *data++;
    case 6:
        res <<= 8;
        res |= *data++;
    case 7:
        res <<= 8;
        res |= *data++;
    case 8:
        res <<= 8;
        res |= *data;
        break;
    }

    return res;
}

void encode_block(const unsigned char* block, unsigned int size, char* res) {
    uint64_t num = uint_8be_to_64(block, size);
    int i = encoded_block_sizes[size];
    while (i--) {
        uint64_t remainder = num % alphabet_size;
        num /= alphabet_size;
        res[i] = alphabet[remainder];
    }
}
