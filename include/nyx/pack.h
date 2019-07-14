#ifndef NYX_PACK_H
#define NYX_PACK_H

int nyx_pack_bit(int bit, void *buffer, uint64_t buf_size, uint64_t *offset);
int nyx_pack_bits(const void *bits, uint64_t bit_len, void *buffer, uint64_t buf_size, uint64_t *offset);
int nyx_pack_zeros(uint64_t len, const void *buffer, uint64_t buf_size, uint64_t *offset);

int nyx_unpack_bit(int *bit, const void *buffer, uint64_t buf_size, uint64_t *offset);
int nyx_unpack_bits(void *bits, uint64_t bit_len, const void *buffer, uint64_t buf_size, uint64_t *offset);
int nyx_unpack_zeros(uint64_t *len, const void *buffer, uint64_t buf_size, uint64_t *offset);

int nyx_pack_i8(int8_t i8, void *buffer, uint64_t buf_size, uint64_t *offset);
int nyx_pack_u8(uint8_t u8, void *buffer, uint64_t buf_size, uint64_t *offset);
int nyx_pack_i16(int16_t i16, void *buffer, uint64_t buf_size, uint64_t *offset);
int nyx_pack_u16(uint16_t u16, void *buffer, uint64_t buf_size, uint64_t *offset);
int nyx_pack_i32(int32_t i32, void *buffer, uint64_t buf_size, uint64_t *offset);
int nyx_pack_u32(uint32_t u32, void *buffer, uint64_t buf_size, uint64_t *offset);
int nyx_pack_i64(int64_t i64, void *buffer, uint64_t buf_size, uint64_t *offset);
int nyx_pack_u64(uint64_t u64, void *buffer, uint64_t buf_size, uint64_t *offset);

int nyx_unpack_i8(int8_t *i8, const void *buffer, uint64_t buf_size, uint64_t *offset);
int nyx_unpack_u8(uint8_t *u8, const void *buffer, uint64_t buf_size, uint64_t *offset);
int nyx_unpack_i16(int16_t *i16, const void *buffer, uint64_t buf_size, uint64_t *offset);
int nyx_unpack_u16(uint16_t *u16, const void *buffer, uint64_t buf_size, uint64_t *offset);
int nyx_unpack_i32(int32_t *i32, const void *buffer, uint64_t buf_size, uint64_t *offset);
int nyx_unpack_u32(uint32_t *u32, const void *buffer, uint64_t buf_size, uint64_t *offset);
int nyx_unpack_i64(int64_t *i64, const void *buffer, uint64_t buf_size, uint64_t *offset);
int nyx_unpack_u64(uint64_t *u64, const void *buffer, uint64_t buf_size, uint64_t *offset);

int nyx_pack_ipak(int64_t i, void *buffer, uint64_t buf_size, uint64_t *offset);
int nyx_pack_upak(int64_t i, void *buffer, uint64_t buf_size, uint64_t *offset);

int nyx_unpack_ipak(int64_t *i, const void *buffer, uint64_t buf_size, uint64_t *offset);
int nyx_unpack_upak(int64_t *i, const void *buffer, uint64_t buf_size, uint64_t *offset);

#endif
