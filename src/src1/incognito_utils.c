#include "os.h"
#include "incognito_types.h"
#include "incognito_api.h"
#include "incognito_vars.h"


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

static void encode_block(const unsigned char* block, unsigned int size, char* res) {
    uint64_t num = uint_8be_to_64(block, size);
    int i = encoded_block_sizes[size];
    while (i--) {
        uint64_t remainder = num % alphabet_size;
        num /= alphabet_size;
        res[i] = alphabet[remainder];
    }
}

int incognito_base58_public_key(char* str_b58, unsigned char* view, unsigned char* spend,
    unsigned char is_subbadress, unsigned char* paymanetID) {
    unsigned char data[72 + 8];
    unsigned int offset;
    unsigned int prefix;

    offset = 0;

    os_memmove(data + offset, spend, 32);
    os_memmove(data + offset + 32, view, 32);
    offset += 64;

    incognito_keccak_F(data, offset, G_incognito_vstate.mlsagH);
    os_memmove(data + offset, G_incognito_vstate.mlsagH, 4);
    offset += 4;

    unsigned int full_block_count = (offset) / FULL_BLOCK_SIZE;
    unsigned int last_block_size = (offset) % FULL_BLOCK_SIZE;
    for (size_t i = 0; i < full_block_count; ++i) {
        encode_block(data + i * FULL_BLOCK_SIZE, FULL_BLOCK_SIZE,
            &str_b58[i * FULL_ENCODED_BLOCK_SIZE]);
    }

    if (0 < last_block_size) {
        encode_block(data + full_block_count * FULL_BLOCK_SIZE, last_block_size,
            &str_b58[full_block_count * FULL_ENCODED_BLOCK_SIZE]);
    }
    str_b58[ADDR_LEN] = '\0';


    return 0;
}
