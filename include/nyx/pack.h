#ifndef NYX_PACK_H
#define NYX_PACK_H

int nyx_pack_bit(void *buffer, uint64_t buf_len, uint64_t *offset, int bit);
int nyx_pack_bits(void *restrict buffer, uint64_t buf_len, uint64_t *offset, const void *bits, uint64_t bits_len);
int nyx_pack_zeros(void *buffer, uint64_t buf_len, uint64_t *offset, uint64_t len);

int nyx_unpack_bit(const void *buffer, uint64_t buf_len, uint64_t *offset, int *bit);
int nyx_unpack_bits(const void *buffer, uint64_t buf_len, uint64_t *offset, void *restrict bits, uint64_t bits_len);
int nyx_unpack_zeros(const void *buffer, uint64_t buf_len, uint64_t *offset, uint64_t *len);

int nyx_pack_i8(void *buffer, uint64_t buf_len, uint64_t *offset, int8_t i8);
int nyx_pack_u8(void *buffer, uint64_t buf_len, uint64_t *offset, uint8_t u8);
int nyx_pack_i16(void *buffer, uint64_t buf_len, uint64_t *offset, int16_t i16);
int nyx_pack_u16(void *buffer, uint64_t buf_len, uint64_t *offset, uint16_t u16);
int nyx_pack_i32(void *buffer, uint64_t buf_len, uint64_t *offset, int32_t i32);
int nyx_pack_u32(void *buffer, uint64_t buf_len, uint64_t *offset, uint32_t u32);
int nyx_pack_i64(void *buffer, uint64_t buf_len, uint64_t *offset, int64_t i64);
int nyx_pack_u64(void *buffer, uint64_t buf_len, uint64_t *offset, uint64_t u64);

int nyx_unpack_i8(const void *buffer, uint64_t buf_len, uint64_t *offset, int8_t *i8);
int nyx_unpack_u8(const void *buffer, uint64_t buf_len, uint64_t *offset, uint8_t *u8);
int nyx_unpack_i16(const void *buffer, uint64_t buf_len, uint64_t *offset, int16_t *i16);
int nyx_unpack_u16(const void *buffer, uint64_t buf_len, uint64_t *offset, uint16_t *u16);
int nyx_unpack_i32(const void *buffer, uint64_t buf_len, uint64_t *offset, int32_t *i32);
int nyx_unpack_u32(const void *buffer, uint64_t buf_len, uint64_t *offset, uint32_t *u32);
int nyx_unpack_i64(const void *buffer, uint64_t buf_len, uint64_t *offset, int64_t *i64);
int nyx_unpack_u64(const void *buffer, uint64_t buf_len, uint64_t *offset, uint64_t *u64);

int nyx_pack_ipak(void *buffer, uint64_t buf_len, uint64_t *offset, int64_t i);
int nyx_pack_upak(void *buffer, uint64_t buf_len, uint64_t *offset, int64_t i);

int nyx_unpack_ipak(const void *buffer, uint64_t buf_len, uint64_t *offset, int64_t *i);
int nyx_unpack_upak(const void *buffer, uint64_t buf_len, uint64_t *offset, int64_t *i);

#endif
