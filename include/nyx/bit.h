#ifndef NYX_BIT_H
#define NYX_BIT_H

#include <stdint.h>

int nyx_get_bit_unsafe(const void *bits, uint64_t bit);
int nyx_get_bit(const void *bits, uint64_t bits_len, uint64_t bit, int *value);

void nyx_set_bit_unsafe(void *bits, uint64_t bit, int value);
int nyx_set_bit(void *bits, uint64_t bits_len, uint64_t bit, int value);

void nyx_set_bits_unsafe(void *bits, uint64_t offset, int value, uint64_t len);
int nyx_set_bits(void *bits, uint64_t bits_len, uint64_t offset, int value, uint64_t len);

void nyx_copy_bits_unsafe(void *restrict dst_bits, uint64_t offset, const void *src, uint64_t len);
int nyx_copy_bits(void *restrict dst_bits, uint64_t dst_len, uint64_t offset, const void *src, uint64_t len);

void nyx_i16_to_bytes(uint8_t *bytes, int16_t i16);
void nyx_u16_to_bytes(uint8_t *bytes, uint16_t u16);
void nyx_i32_to_bytes(uint8_t *bytes, int32_t i32);
void nyx_u32_to_bytes(uint8_t *bytes, uint32_t u32);
void nyx_i64_to_bytes(uint8_t *bytes, int64_t i64);
void nyx_u64_to_bytes(uint8_t *bytes, uint64_t u64);

int16_t nyx_bytes_to_i16(const uint8_t *bytes);
uint16_t nyx_bytes_to_u16(const uint8_t *bytes);
int32_t nyx_bytes_to_i32(const uint8_t *bytes);
uint32_t nyx_bytes_to_u32(const uint8_t *bytes);
int64_t nyx_bytes_to_i64(const uint8_t *bytes);
uint64_t nyx_bytes_to_u64(const uint8_t *bytes);

#endif
