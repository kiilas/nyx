#include "nyx/nyx.h"

int nyx_pack_bit(void *buffer, uint64_t buf_len, uint64_t *offset, int bit) {
    if(nyx_set_bit(buffer, buf_len, *offset, bit))
        return -1;
    ++*offset;
    return 0;
}

int nyx_pack_bits(void *restrict buffer, uint64_t buf_len, uint64_t *offset, const void *bits, uint64_t bits_len) {
    if(nyx_copy_bits(buffer, buf_len, *offset, bits, bits_len, 0, bits_len))
        return -1;
    *offset += bits_len;
    return 0;
}

int nyx_pack_zeros(void *buffer, uint64_t buf_len, uint64_t *offset, uint64_t len) {
    if(nyx_set_bits(buffer, buf_len, *offset, 0, len))
        return -1;
    *offset += len;
    return 0;
}

int nyx_unpack_bit(const void *buffer, uint64_t buf_len, uint64_t *offset, int *bit) {
    if(nyx_get_bit(buffer, buf_len, *offset, bit))
        return -1;
    ++*offset;
    return 0;
}

int nyx_unpack_bits(const void *buffer, uint64_t buf_len, uint64_t *offset, void *restrict bits, uint64_t bits_len) {
    if(nyx_copy_bits(bits, bits_len, 0, buffer, buf_len, *offset, bits_len))
        return -1;
    *offset += bits_len;
    return 0;
}

int nyx_unpack_zeros(const void *buffer, uint64_t buf_len, uint64_t *offset, uint64_t *len) {
    for(*len=0; *len+*offset < buf_len; ++*len)
        if(nyx_get_bit_unsafe(buffer, *len+*offset))
        {
            *offset += *len;
            return 0;
        }
    return -1;
}

int nyx_pack_i8(void *buffer, uint64_t buf_len, uint64_t *offset, int8_t i8) {
    return nyx_pack_bits(buffer, buf_len, offset, &i8, 8);
}

int nyx_pack_u8(void *buffer, uint64_t buf_len, uint64_t *offset, uint8_t u8) {
    return nyx_pack_bits(buffer, buf_len, offset, &u8, 8);
}

int nyx_pack_i16(void *buffer, uint64_t buf_len, uint64_t *offset, int16_t i16) {
    uint8_t bytes[2];

    nyx_i16_to_bytes(bytes, i16);
    return nyx_pack_bits(buffer, buf_len, offset, bytes, 16);
}

int nyx_pack_u16(void *buffer, uint64_t buf_len, uint64_t *offset, uint16_t u16) {
    uint8_t bytes[2];

    nyx_u16_to_bytes(bytes, u16);
    return nyx_pack_bits(buffer, buf_len, offset, bytes, 16);
}

int nyx_pack_i32(void *buffer, uint64_t buf_len, uint64_t *offset, int32_t i32) {
    uint8_t bytes[4];

    nyx_i32_to_bytes(bytes, i32);
    return nyx_pack_bits(buffer, buf_len, offset, bytes, 32);
}

int nyx_pack_u32(void *buffer, uint64_t buf_len, uint64_t *offset, uint32_t u32) {
    uint8_t bytes[4];

    nyx_u32_to_bytes(bytes, u32);
    return nyx_pack_bits(buffer, buf_len, offset, bytes, 32);
}

int nyx_pack_i64(void *buffer, uint64_t buf_len, uint64_t *offset, int64_t i64) {
    uint8_t bytes[8];

    nyx_i64_to_bytes(bytes, i64);
    return nyx_pack_bits(buffer, buf_len, offset, bytes, 64);
}

int nyx_pack_u64(void *buffer, uint64_t buf_len, uint64_t *offset, uint64_t u64) {
    uint8_t bytes[8];

    nyx_u64_to_bytes(bytes, u64);
    return nyx_pack_bits(buffer, buf_len, offset, bytes, 64);
}

int nyx_unpack_i8(const void *buffer, uint64_t buf_len, uint64_t *offset, int8_t *i8) {
    return nyx_unpack_bits(buffer, buf_len, offset, i8, 8);
}

int nyx_unpack_u8(const void *buffer, uint64_t buf_len, uint64_t *offset, uint8_t *u8) {
    return nyx_unpack_bits(buffer, buf_len, offset, u8, 8);
}

int nyx_unpack_i16(const void *buffer, uint64_t buf_len, uint64_t *offset, int16_t *i16) {
    uint8_t bytes[2];

    if(nyx_unpack_bits(buffer, buf_len, offset, bytes, 16))
        return -1;
    *i16 = nyx_bytes_to_i16(bytes);
    return 0;
}

int nyx_unpack_u16(const void *buffer, uint64_t buf_len, uint64_t *offset, uint16_t *u16) {
    uint8_t bytes[2];

    if(nyx_unpack_bits(buffer, buf_len, offset, bytes, 16))
        return -1;
    *u16 = nyx_bytes_to_u16(bytes);
    return 0;
}

int nyx_unpack_i32(const void *buffer, uint64_t buf_len, uint64_t *offset, int32_t *i32) {
    uint8_t bytes[4];

    if(nyx_unpack_bits(buffer, buf_len, offset, bytes, 32))
        return -1;
    *i32 = nyx_bytes_to_i32(bytes);
    return 0;
}

int nyx_unpack_u32(const void *buffer, uint64_t buf_len, uint64_t *offset, uint32_t *u32) {
    uint8_t bytes[4];

    if(nyx_unpack_bits(buffer, buf_len, offset, bytes, 32))
        return -1;
    *u32 = nyx_bytes_to_u32(bytes);
    return 0;
}

int nyx_unpack_i64(const void *buffer, uint64_t buf_len, uint64_t *offset, int64_t *i64) {
    uint8_t bytes[8];

    if(nyx_unpack_bits(buffer, buf_len, offset, bytes, 64))
        return -1;
    *i64 = nyx_bytes_to_i64(bytes);
    return 0;
}

int nyx_unpack_u64(const void *buffer, uint64_t buf_len, uint64_t *offset, uint64_t *u64) {
    uint8_t bytes[8];

    if(nyx_unpack_bits(buffer, buf_len, offset, bytes, 64))
        return -1;
    *u64 = nyx_bytes_to_u64(bytes);
    return 0;
}

int nyx_pack_ipak(void *buffer, uint64_t buf_len, uint64_t *offset, int64_t i) {
    uint64_t *const offset_bkp = offset;

    int bit = i < 0;

    if(nyx_pack_bit(buffer, buf_len, offset, bit))
        return -1;
    if(nyx_pack_upak(buffer, buf_len, offset, i < 0 ? -i-1 : i))
    {
        *offset = *offset_bkp;
        return -1;
    }
    return 0;
}

int nyx_pack_upak(void *buffer, uint64_t buf_len, uint64_t *offset, int64_t i) {
    uint64_t *const offset_bkp = offset;

    uint64_t mantissa = i + 1;
    int mantissa_bits = 0;
    int n;
    
    if(!mantissa)
        return -1;
    n = mantissa;
    while(n)
    {
        n /= 2;
        ++mantissa_bits;
    }

    if(nyx_pack_zeros(buffer, buf_len, offset, mantissa_bits-1))
        return -1;
    n = mantissa;
    while(n)
    {
        if(nyx_pack_bit(buffer, buf_len, offset, n%2))
        {
            *offset = *offset_bkp;
            return -1;
        }
        n /= 2;
    }
    return 0;
}

int nyx_unpack_ipak(const void *buffer, uint64_t buf_len, uint64_t *offset, int64_t *i) {
    uint64_t *const offset_bkp = offset;

    int64_t unsigned_part;
    int bit;

    if(nyx_unpack_bit(buffer, buf_len, offset, &bit))
        return -1;
    if(nyx_unpack_upak(buffer, buf_len, offset, &unsigned_part))
    {
        *offset = *offset_bkp;
        return -1;
    }
    *i = bit ? -unsigned_part-1 : unsigned_part;
    return 0;
}

int nyx_unpack_upak(const void *buffer, uint64_t buf_len, uint64_t *offset, int64_t *i) {
    uint64_t *const offset_bkp = offset;

    uint64_t mantissa_bits;
    unsigned pos;

    if(nyx_unpack_zeros(buffer, buf_len, offset, &mantissa_bits))
        return -1;
    *i = 0;
    for(pos=0; pos<mantissa_bits; ++pos)
    {
        int bit;

        if(nyx_unpack_bit(buffer, buf_len, offset, &bit))
        {
            *offset = *offset_bkp;
            return -1;
        }
        *i |= bit << pos;
    }
    return 0;
}
