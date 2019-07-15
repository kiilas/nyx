#include "nyx/nyx.h"

int nyx_get_bit_unsafe(const void *bits, uint64_t bit) {
    const uint8_t *byte = (uint8_t *)bits + (bit/8);

    return *byte >> bit%8;
}

int nyx_get_bit(const void *bits, uint64_t bits_len, uint64_t bit, int *value) {
    if(bit >= bits_len)
        return -1;
    *value = nyx_get_bit_unsafe(bits, bit);
    return 0;
}

void nyx_set_bit_unsafe(void *bits, uint64_t bit, int value) {
    uint8_t *byte = (uint8_t *)bits + (bit/8);

    *byte &= ~(1 << bit%8);
    *byte |= value%2 << bit%8;
}

int nyx_set_bit(void *bits, uint64_t bits_len, uint64_t bit, int value) {
    if(bit >= bits_len)
        return -1;
    nyx_set_bit_unsafe(bits, bit, value);
    return 0;
}

void nyx_set_bits_unsafe(void *bits, uint64_t offset, int value, uint64_t len) {
    uint64_t i;

    for(i=0; i<len; ++i)
        nyx_set_bit_unsafe(bits, offset+i, value);
}

int nyx_set_bits(void *bits, uint64_t bits_len, uint64_t offset, int value, uint64_t len) {
    if(offset+len > bits_len && len)
        return -1;
    nyx_set_bits_unsafe(bits, offset, value, len);
    return 0;
}

void nyx_copy_bits_unsafe(void *restrict dst_bits, uint64_t offset, const void *src, uint64_t len) {
    uint64_t i;

    for(i=0; i<len; ++i)
    {
        int value = nyx_get_bit_unsafe(src, i);

        nyx_set_bit_unsafe(dst_bits, offset+i, value);
    }
}

int nyx_copy_bits(void *restrict dst_bits, uint64_t dst_len, uint64_t offset, const void *src, uint64_t len) {
    if(offset+len > dst_len && len)
        return -1;
    nyx_copy_bits_unsafe(dst_bits, offset, src, len);
    return 0;
}

void nyx_i16_to_bytes(uint8_t *bytes, int16_t i16) {
    bytes[0] = i16 & 0xff;
    bytes[1] = i16 >> 8 & 0xff;
}

void nyx_u16_to_bytes(uint8_t *bytes, uint16_t u16) {
    bytes[0] = u16 & 0xff;
    bytes[1] = u16 >> 8 & 0xff;
}

void nyx_i32_to_bytes(uint8_t *bytes, int32_t i32) {
    bytes[0] = i32 & 0xff;
    bytes[1] = i32 >> 8 & 0xff;
    bytes[2] = i32 >> 16 & 0xff;
    bytes[3] = i32 >> 24 & 0xff;
}

void nyx_u32_to_bytes(uint8_t *bytes, uint32_t u32) {
    bytes[0] = u32 & 0xff;
    bytes[1] = u32 >> 8 & 0xff;
    bytes[2] = u32 >> 16 & 0xff;
    bytes[3] = u32 >> 24 & 0xff;
}

void nyx_i64_to_bytes(uint8_t *bytes, int64_t i64) {
    bytes[0] = i64 & 0xff;
    bytes[1] = i64 >> 8 & 0xff;
    bytes[2] = i64 >> 16 & 0xff;
    bytes[3] = i64 >> 24 & 0xff;
    bytes[4] = i64 >> 32 & 0xff;
    bytes[5] = i64 >> 40 & 0xff;
    bytes[6] = i64 >> 48 & 0xff;
    bytes[7] = i64 >> 56 & 0xff;
}

void nyx_u64_to_bytes(uint8_t *bytes, uint64_t u64) {
    bytes[0] = u64 & 0xff;
    bytes[1] = u64 >> 8 & 0xff;
    bytes[2] = u64 >> 16 & 0xff;
    bytes[3] = u64 >> 24 & 0xff;
    bytes[4] = u64 >> 32 & 0xff;
    bytes[5] = u64 >> 40 & 0xff;
    bytes[6] = u64 >> 48 & 0xff;
    bytes[7] = u64 >> 56 & 0xff;
}

int16_t nyx_bytes_to_i16(const uint8_t *bytes) {
    int16_t i16;

    i16 = (int16_t)bytes[0];
    i16 |= (int16_t)bytes[1] << 8;
    return i16;
}

uint16_t nyx_bytes_to_u16(const uint8_t *bytes) {
    uint16_t u16;

    u16 = (uint16_t)bytes[0];
    u16 |= (uint16_t)bytes[1] << 8;
    return u16;
}

int32_t nyx_bytes_to_i32(const uint8_t *bytes) {
    int32_t i32;

    i32 = (int32_t)bytes[0];
    i32 |= (int32_t)bytes[1] << 8;
    i32 |= (int32_t)bytes[2] << 16;
    i32 |= (int32_t)bytes[3] << 24;
    return i32;
}

uint32_t nyx_bytes_to_u32(const uint8_t *bytes) {
    uint32_t u32;

    u32 = (uint32_t)bytes[0];
    u32 |= (uint32_t)bytes[1] << 8;
    u32 |= (uint32_t)bytes[2] << 16;
    u32 |= (uint32_t)bytes[3] << 24;
    return u32;
}

int64_t nyx_bytes_to_i64(const uint8_t *bytes) {
    int64_t i64;

    i64 = (int64_t)bytes[0];
    i64 |= (int64_t)bytes[1] << 8;
    i64 |= (int64_t)bytes[2] << 16;
    i64 |= (int64_t)bytes[3] << 24;
    i64 |= (int64_t)bytes[4] << 32;
    i64 |= (int64_t)bytes[5] << 40;
    i64 |= (int64_t)bytes[6] << 48;
    i64 |= (int64_t)bytes[7] << 56;
    return i64;
}

uint64_t nyx_bytes_to_u64(const uint8_t *bytes) {
    uint64_t u64;

    u64 = (uint64_t)bytes[0];
    u64 |= (uint64_t)bytes[1] << 8;
    u64 |= (uint64_t)bytes[2] << 16;
    u64 |= (uint64_t)bytes[3] << 24;
    u64 |= (uint64_t)bytes[4] << 32;
    u64 |= (uint64_t)bytes[5] << 40;
    u64 |= (uint64_t)bytes[6] << 48;
    u64 |= (uint64_t)bytes[7] << 56;
    return u64;
}