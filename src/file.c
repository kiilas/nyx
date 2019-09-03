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

static int read_bytes(NYX_FILE *file, void *ptr, size_t size) {
    if(file->mode != NYX_FILE_READ)
        return -1;
    if(!size)
        return 0;
    return fread(ptr, size, 1, file->fd) ? 0 : -1;
}

static int read_bits(NYX_FILE *file, void *ptr, size_t size) {
    uint8_t buf[BUF_SIZE];
    size_t offset;

    if(file->mode != NYX_FILE_READ)
        return -1;
    if(!size)
        return 0;
    offset = file->bit_pos;
    if(offset > size)
        offset = size;
    *(uint8_t *)ptr = file->bit_buf & (1<<offset) - 1;
    file->bit_pos -= offset;
    file->bit_buf >>= offset;
    while(offset < size)
    {
        size_t chunk = size - offset;
        size_t bytes_read;

        if(chunk > 8 * BUF_SIZE)
            chunk = 8 * BUF_SIZE;
        bytes_read = (chunk+7) / 8;
        if(read_bytes(file, buf, bytes_read))
            return -1;
        nyx_copy_bits_unsafe(ptr, offset, buf, 0, chunk);
        if(chunk % 8)
        {
            file->bit_pos = 8 - chunk%8;
            file->bit_buf = ((uint8_t *)buf)[bytes_read-1] >> chunk%8;
        }
        offset += chunk;
    }
    return 0;
}

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
    const char *mode_arg;

    file = calloc(1, sizeof(NYX_FILE));
    if(!file)
        return 0;

    if(mode == NYX_FILE_READ)
        mode_arg = read;
    else if(mode == NYX_FILE_WRITE)
        mode_arg = write;
    else if(mode == NYX_FILE_APPEND)
        mode_arg = append;
    else
        return 0;
    file->fd = fopen(path, mode_arg);
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

int nyx_file_read(NYX_FILE *file, void *ptr, size_t size) {
    if(file->format == NYX_FILE_BYTE)
        return read_bytes(file, ptr, size);
    if(file->format == NYX_FILE_BIT)
        return read_bits(file, ptr, 8*size);
    return -1;
}

int nyx_file_write(NYX_FILE *file, const void *ptr, size_t size) {
    if(file->format == NYX_FILE_BYTE)
        return write_bytes(file, ptr, size);
    if(file->format == NYX_FILE_BIT)
        return write_bits(file, ptr, 8*size);
    return -1;
}

int nyx_file_read_bit(NYX_FILE *file, int *bit) {
    uint8_t byte;

    if(file->mode != NYX_FILE_READ)
        return -1;
    if(file->bit_pos)
    {
        *bit = file->bit_buf & 1;
        --file->bit_pos;
        file->bit_buf >>= 1;
        return 0;
    }
    if(read_bytes(file, &byte, 1))
        return -1;
    *bit = byte & 1;
    file->bit_pos = 7;
    file->bit_buf = byte >> 1;
    return 0;
}

int nyx_file_read_bits(NYX_FILE *file, void *ptr, size_t size) {
    return read_bits(file, ptr, size);
}

int nyx_file_read_zeros(NYX_FILE *file, size_t *len) {
    int bit;

    *len = 0;
    while(1)
    {
        if(nyx_file_read_bit(file, &bit))
            return -1;
        if(bit)
            return 0;
        ++*len;
    }
}

int nyx_file_write_bit(NYX_FILE *file, int bit) {
    if(file->mode != NYX_FILE_WRITE && file->mode != NYX_FILE_APPEND)
        return -1;
    file->bit_buf = 2*file->bit_buf + bit;
    ++file->bit_pos;
    if(file->bit_pos == 8)
        if(flush_bit_buf(file))
            return -1;
    return 0;
}

int nyx_file_write_bits(NYX_FILE *file, const void *ptr, size_t size) {
    return write_bits(file, ptr, size);
}

int nyx_file_write_zeros(NYX_FILE *file, size_t len) {
    size_t i;

    for(i=0; i<len; ++i)
        if(nyx_file_write_bit(file, 0))
            return -1;
    if(nyx_file_write_bit(file, 1))
        return -1;
    return 0;
}

int nyx_file_read_i8(NYX_FILE *file, int8_t *i8) {
    return nyx_file_read(file, i8, 1);
}

int nyx_file_read_u8(NYX_FILE *file, uint8_t *u8) {
    return nyx_file_read(file, u8, 1);
}

int nyx_file_read_i16(NYX_FILE *file, int16_t *i16) {
    uint8_t bytes[2];

    if(nyx_file_read(file, bytes, 2))
        return -1;
    *i16 = nyx_bytes_to_i16(bytes);
    return 0;
}

int nyx_file_read_u16(NYX_FILE *file, uint16_t *u16) {
    uint8_t bytes[2];

    if(nyx_file_read(file, bytes, 2))
        return -1;
    *u16 = nyx_bytes_to_u16(bytes);
    return 0;
}

int nyx_file_read_i32(NYX_FILE *file, int32_t *i32) {
    uint8_t bytes[4];

    if(nyx_file_read(file, bytes, 4))
        return -1;
    *i32 = nyx_bytes_to_i32(bytes);
    return 0;
}

int nyx_file_read_u32(NYX_FILE *file, uint32_t *u32) {
    uint8_t bytes[4];

    if(nyx_file_read(file, bytes, 4))
        return -1;
    *u32 = nyx_bytes_to_u32(bytes);
    return 0;
}

int nyx_file_read_i64(NYX_FILE *file, int64_t *i64) {
    uint8_t bytes[8];

    if(nyx_file_read(file, bytes, 8))
        return -1;
    *i64 = nyx_bytes_to_i64(bytes);
    return 0;
}

int nyx_file_read_u64(NYX_FILE *file, uint64_t *u64) {
    uint8_t bytes[8];

    if(nyx_file_read(file, bytes, 8))
        return -1;
    *u64 = nyx_bytes_to_u64(bytes);
    return 0;
}

int nyx_file_read_ipak(NYX_FILE *file, int64_t *i) {
    int64_t unsigned_part;
    int bit;

    if(nyx_file_read_bit(file, &bit))
        return -1;
    if(nyx_file_read_upak(file, &unsigned_part))
        return -1;
    *i = bit ? -unsigned_part-1 : unsigned_part;
    return 0;
}

int nyx_file_read_upak(NYX_FILE *file, int64_t *i) {
    uint64_t mantissa_bits;
    unsigned pos;

    if(nyx_file_read_zeros(file, &mantissa_bits))
        return -1;
    *i = 1;
    for(pos=0; pos<mantissa_bits; ++pos)
    {
        int bit;

        if(nyx_file_read_bit(file, &bit))
            return -1;
        *i = 2 * *i + bit;
    }
    --*i;
    return 0;
}

int nyx_file_read_cstring(NYX_FILE *file, char *str, size_t n) {
    size_t i;

    for(i=0; i<n; ++i)
    {
        if(nyx_file_read(file, str+i, 1))
            return -1;
        if(!str[i])
            break;
    }
    return i;
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
