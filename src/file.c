#include "nyx/nyx.h"

#include <string.h>

#define BUF_SIZE 4096

struct NYX_FILE {
    FILE    *fd;
    int      mode;
    int      format;
    int      bit_pos;
    uint8_t  bit_buf;
};

static int write_bytes(NYX_FILE *file, const void *ptr, size_t size) {
    if(file->mode != NYX_FILE_WRITE && file->mode != NYX_FILE_APPEND)
        return -1;
    if(!size)
        return 0;
    return fwrite(ptr, size, 1, file->fd) ? 0 : -1;
}

static int flush_bit_buf(NYX_FILE *file) {
    if(!file->bit_pos)
        return 0;
    file->bit_buf &= (1<<file->bit_pos) - 1;
    write_bytes(file, &file->bit_buf, 1);
    file->bit_pos = 0;
    return 0;
}

static int write_bits(NYX_FILE *file, const void *ptr, size_t size) {
    uint8_t buf[BUF_SIZE];
    size_t offset = 0;

    if(file->mode != NYX_FILE_WRITE && file->mode != NYX_FILE_APPEND)
        return -1;
    while(offset < size)
    {
        const size_t avail = BUF_SIZE*8 - file->bit_pos;

        size_t chunk = size - offset;
        size_t buf_used;

        if(chunk > avail)
            chunk = avail;
        buf[0] = file->bit_buf;
        nyx_copy_bits_unsafe(buf, file->bit_pos, ptr, offset, chunk);
        buf_used = file->bit_pos + chunk;
        if(write_bytes(file, buf, buf_used/8))
            return -1;
        file->bit_pos = buf_used % 8;
        if(file->bit_pos)
            file->bit_buf = buf[buf_used/8];
        offset += chunk;
    }
    if(file->mode == NYX_FILE_BYTE && flush_bit_buf(file))
        return -1;
    return 0;
}

NYX_FILE *nyx_file_open(const char *path, int mode, int format) {
    const char *read = "rb";
    const char *write = "wb";
    const char *append = "ab";

    NYX_FILE *file;
    const char **mode_arg;

    file = calloc(1, sizeof(NYX_FILE));
    if(!file)
        return 0;

    if(mode == NYX_FILE_READ)
        mode_arg = &read;
    else if(mode == NYX_FILE_WRITE)
        mode_arg = &write;
    else if(mode == NYX_FILE_APPEND)
        mode_arg = &append;
    else
        return 0;
    file->fd = fopen(path, *mode_arg);
    if(!file->fd)
    {
        nyx_file_close(file);
        return 0;
    }
    file->mode = mode;
    file->format = format;
    return file;
}

void nyx_file_close(NYX_FILE *file) {
    if(!file)
        return;
    flush_bit_buf(file);
    if(file->fd)
        fclose(file->fd);
    free(file);
}

int nyx_file_write(NYX_FILE *file, const void *ptr, size_t size) {
    if(file->format == NYX_FILE_BYTE)
        return write_bytes(file, ptr, size);
    if(file->format == NYX_FILE_BIT)
        return write_bits(file, ptr, 8*size);
    return -1;
}

int nyx_file_write_bits(NYX_FILE *file, const void *ptr, size_t size) {
    return write_bits(file, ptr, size);
}

int nyx_file_write_i8(NYX_FILE *file, int8_t i8) {
    return nyx_file_write(file, &i8, 1);
}

int nyx_file_write_u8(NYX_FILE *file, uint8_t u8) {
    return nyx_file_write(file, &u8, 1);
}

int nyx_file_write_i16(NYX_FILE *file, int16_t i16) {
    uint8_t bytes[2];

    nyx_i16_to_bytes(bytes, i16);
    return nyx_file_write(file, bytes, 2);
}

int nyx_file_write_u16(NYX_FILE *file, uint16_t u16) {
    uint8_t bytes[2];

    nyx_u16_to_bytes(bytes, u16);
    return nyx_file_write(file, bytes, 2);
}

int nyx_file_write_i32(NYX_FILE *file, int32_t i32) {
    uint8_t bytes[4];

    nyx_i32_to_bytes(bytes, i32);
    return nyx_file_write(file, bytes, 4);
}
int nyx_file_write_u32(NYX_FILE *file, uint32_t u32) {
    uint8_t bytes[4];

    nyx_u32_to_bytes(bytes, u32);
    return nyx_file_write(file, bytes, 4);
}

int nyx_file_write_i64(NYX_FILE *file, int64_t i64) {
    uint8_t bytes[8];

    nyx_i64_to_bytes(bytes, i64);
    return nyx_file_write(file, bytes, 8);
}

int nyx_file_write_u64(NYX_FILE *file, uint64_t u64) {
    uint8_t bytes[8];

    nyx_u64_to_bytes(bytes, u64);
    return nyx_file_write(file, bytes, 8);
}

int nyx_file_write_ipak(NYX_FILE *file, int64_t i) {
    uint8_t buffer[16];
    uint64_t offset = 0;

    if(nyx_pack_ipak(buffer, 128, &offset, i))
        return -1;
    return write_bits(file, buffer, offset);
}

int nyx_file_write_upak(NYX_FILE *file, int64_t i) {
    uint8_t buffer[16];
    uint64_t offset = 0;
    
    if(nyx_pack_upak(buffer, 128, &offset, i))
        return -1;
    return write_bits(file, buffer, offset);
}

int nyx_file_write_cstring(NYX_FILE *file, const char *str, size_t n) {
    size_t len = strnlen(str, n);

    return nyx_file_write(file, str, len);
}
