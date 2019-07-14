#include "nyx/nyx.h"

int nyx_pack_i8(int8_t i8, void *buffer, uint64_t buf_size, uint64_t *offset) {
    return nyx_pack_bits(&i8, 8, buffer, buf_size, offset);
}

int nyx_pack_u8(uint8_t u8, void *buffer, uint64_t buf_size, uint64_t *offset) {
    return nyx_pack_bits(&u8, 8, buffer, buf_size, offset);
}

int nyx_pack_i16(int16_t i16, void *buffer, uint64_t buf_size, uint64_t *offset) {
    uint8_t bytes[2];

    nyx_i16_to_bytes(i16, bytes);
    return nyx_pack_bits(bytes, 16, buffer, buf_size, offset);
}

int nyx_pack_u16(uint16_t u16, void *buffer, uint64_t buf_size, uint64_t *offset) {
    uint8_t bytes[2];

    nyx_u16_to_bytes(u16, bytes);
    return nyx_pack_bits(bytes, 16, buffer, buf_size, offset);
}

int nyx_pack_i32(int32_t i32, void *buffer, uint64_t buf_size, uint64_t *offset) {
    uint8_t bytes[4];

    nyx_i32_to_bytes(i32, bytes);
    return nyx_pack_bits(bytes, 32, buffer, buf_size, offset);
}

int nyx_pack_u32(uint32_t u32, void *buffer, uint64_t buf_size, uint64_t *offset) {
    uint8_t bytes[4];

    nyx_u32_to_bytes(u32, bytes);
    return nyx_pack_bits(bytes, 32, buffer, buf_size, offset);
}

int nyx_pack_i64(int64_t i64, void *buffer, uint64_t buf_size, uint64_t *offset) {
    uint8_t bytes[8];

    nyx_i64_to_bytes(i64, bytes);
    return nyx_pack_bits(bytes, 64, buffer, buf_size, offset);
}

int nyx_pack_u64(uint64_t u64, void *buffer, uint64_t buf_size, uint64_t *offset) {
    uint8_t bytes[8];

    nyx_u64_to_bytes(u64, bytes);
    return nyx_pack_bits(bytes, 64, buffer, buf_size, offset);
}

int nyx_unpack_i8(int8_t *i8, const void *buffer, uint64_t buf_size, uint64_t *offset) {
    return nyx_unpack_bits(i8, 8, buffer, buf_size, offset);
}

int nyx_unpack_u8(uint8_t *u8, const void *buffer, uint64_t buf_size, uint64_t *offset) {
    return nyx_unpack_bits(u8, 8, buffer, buf_size, offset);
}

int nyx_unpack_i16(int16_t *i16, const void *buffer, uint64_t buf_size, uint64_t *offset) {
    uint8_t bytes[2];

    if(nyx_unpack_bits(bytes, 16, buffer, buf_size, offset))
        return -1;
    *i16 = nyx_bytes_to_i16(bytes);
    return 0;
}

int nyx_unpack_u16(uint16_t *u16, const void *buffer, uint64_t buf_size, uint64_t *offset) {
    uint8_t bytes[2];

    if(nyx_unpack_bits(bytes, 16, buffer, buf_size, offset))
        return -1;
    *u16 = nyx_bytes_to_u16(bytes);
    return 0;
}

int nyx_unpack_i32(int32_t *i32, const void *buffer, uint64_t buf_size, uint64_t *offset) {
    uint8_t bytes[4];

    if(nyx_unpack_bits(bytes, 32, buffer, buf_size, offset))
        return -1;
    *i32 = nyx_bytes_to_i32(bytes);
    return 0;
}

int nyx_unpack_u32(uint32_t *u32, const void *buffer, uint64_t buf_size, uint64_t *offset) {
    uint8_t bytes[4];

    if(nyx_unpack_bits(bytes, 32, buffer, buf_size, offset))
        return -1;
    *u32 = nyx_bytes_to_u32(bytes);
    return 0;
}

int nyx_unpack_i64(int64_t *i64, const void *buffer, uint64_t buf_size, uint64_t *offset) {
    uint8_t bytes[8];

    if(nyx_unpack_bits(bytes, 64, buffer, buf_size, offset))
        return -1;
    *i64 = nyx_bytes_to_i64(bytes);
    return 0;
}

int nyx_unpack_u64(uint64_t *u64, const void *buffer, uint64_t buf_size, uint64_t *offset) {
    uint8_t bytes[8];

    if(nyx_unpack_bits(bytes, 64, buffer, buf_size, offset))
        return -1;
    *u64 = nyx_bytes_to_u64(bytes);
    return 0;
}

int nyx_pack_ipak(int64_t i, void *buffer, uint64_t buf_size, uint64_t *offset) {
    uint64_t *const offset_bkp = offset;

    int bit = i < 0;

    if(nyx_pack_bit(bit, buffer, buf_size, offset))
        return -1;
    if(nyx_pack_upak(i < 0 ? -i-1 : i, buffer, buf_size, offset))
    {
        offset = offset_bkp;
        return -1;
    }
    return 0;
}

int nyx_pack_upak(int64_t i, void *buffer, uint64_t buf_size, uint64_t *offset) {
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

    if(nyx_pack_zeros(mantissa_bits-1, buffer, buf_size, offset))
        return -1;
    n = mantissa;
    while(n)
    {
        if(nyx_pack_bit(n%2, buffer, buf_size, offset))
        {
            offset = offset_bkp;
            return -1;
        }
        n /= 2;
    }
    return 0;
}

int nyx_unpack_ipak(int64_t *i, const void *buffer, uint64_t buf_size, uint64_t *offset) {
    uint64_t *const offset_bkp = offset;

    int64_t unsigned_part;
    int bit;

    if(nyx_unpack_bit(&bit, buffer, buf_size, offset))
        return -1;
    if(nyx_unpack_upak(&unsigned_part, buffer, buf_size, offset))
    {
        offset = offset_bkp;
        return -1;
    }
    *i = bit ? -unsigned_part-1 : unsigned_part;
    return 0;
}

int nyx_unpack_upak(int64_t *i, const void *buffer, uint64_t buf_size, uint64_t *offset) {
    uint64_t *const offset_bkp = offset;

    uint64_t mantissa_bits;
    unsigned pos;

    if(nyx_unpack_zeros(&mantissa_bits, buffer, buf_size, offset))
        return -1;
    *i = 0;
    for(pos=0; pos<mantissa_bits; ++pos)
    {
        int bit;

        if(nyx_unpack_bit(&bit, buffer, buf_size, offset))
        {
            offset = offset_bkp;
            return -1;
        }
        *i |= bit << pos;
    }
    return 0;
}
