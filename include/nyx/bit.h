#ifndef NYX_BIT_H
#define NYX_BIT_H

#include <stdint.h>

void nyx_i16_to_bytes(int16_t i16, uint8_t *bytes);
void nyx_u16_to_bytes(uint16_t u16, uint8_t *bytes);
void nyx_i32_to_bytes(int32_t i32, uint8_t *bytes);
void nyx_u32_to_bytes(uint32_t u32, uint8_t *bytes);
void nyx_i64_to_bytes(int64_t i64, uint8_t *bytes);
void nyx_u64_to_bytes(uint64_t u64, uint8_t *bytes);

int16_t  nyx_bytes_to_i16(uint8_t *bytes);
uint16_t nyx_bytes_to_u16(uint8_t *bytes);
int32_t  nyx_bytes_to_i32(uint8_t *bytes);
uint32_t nyx_bytes_to_u32(uint8_t *bytes);
int64_t  nyx_bytes_to_i64(uint8_t *bytes);
uint64_t nyx_bytes_to_u64(uint8_t *bytes);

#endif
