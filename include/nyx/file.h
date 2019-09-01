#ifndef NYX_FILE_H
#define NYX_FILE_H

#define NYX_FILE_READ   0
#define NYX_FILE_WRITE  1
#define NYX_FILE_APPEND 2
#define NYX_FILE_BYTE   0
#define NYX_FILE_BIT    1

typedef struct NYX_FILE NYX_FILE;

NYX_FILE *nyx_file_open(const char *path, int mode, int format);
void nyx_file_close(NYX_FILE *file);

int nyx_file_write(NYX_FILE *file, const void *ptr, size_t size);
int nyx_file_write_bits(NYX_FILE *file, const void *ptr, size_t size);

int nyx_file_write_i8(NYX_FILE *file, int8_t i8);
int nyx_file_write_u8(NYX_FILE *file, uint8_t u8);
int nyx_file_write_i16(NYX_FILE *file, int16_t i16);
int nyx_file_write_u16(NYX_FILE *file, uint16_t u16);
int nyx_file_write_i32(NYX_FILE *file, int32_t i32);
int nyx_file_write_u32(NYX_FILE *file, uint32_t u32);
int nyx_file_write_i64(NYX_FILE *file, int64_t i64);
int nyx_file_write_u64(NYX_FILE *file, uint64_t u64);
int nyx_file_write_ipak(NYX_FILE *file, int64_t i);
int nyx_file_write_upak(NYX_FILE *file, int64_t i);
int nyx_file_write_cstring(NYX_FILE *file, const char *str, size_t n);

#endif
