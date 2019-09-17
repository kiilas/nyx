#include "nyx/nyx.h"

int _unicode_decode(const char *str, size_t n, size_t *idx, uint32_t *code) {
    const uint8_t *c = (const uint8_t *)str + *idx;

    if(c[0] < 0x80)
    {
        if(*idx >= n)
            return -1;
        *code = *c;
        *idx += 1;
        return 0;
    }
    if(c[0] < 0xc0)
        return -1;
    if(c[0] < 0xe0)
    {
        if(*idx + 1 >= n)
            return -1;
        if(c[1]>>6 != 2)
            return -1;
        *code = (c[0]&0x1f) << 6 |
                c[1]&0x3f;
        *idx += 2;
        return 0;
    }
    if(c[0] < 0xf0)
    {
        if(*idx + 2 >= n)
            return -1;
        if(c[1]>>6 != 2 || c[2]>>6 != 2)
            return -1;
        *code = (c[0]&0x0f) << 12 |
                (c[1]&0x3f) << 6 |
                c[2]&0x3f;
        *idx += 3;
        return 0;
    }
    if(c[0] < 0xf8)
    {
        if(*idx + 3 >= n)
            return -1;
        if(c[1]>>6 != 2 || c[2]>>6 != 2 || c[3]>>6 != 2)
            return -1;
        *code = (c[0]&0x07) << 18 |
                (c[1]&0x3f) << 12 |
                (c[2]&0x3f) << 6 |
                c[3]&0x3f;
        *idx += 4;
        return 0;
    }
    return -1;
}

int _unicode_encode(char *str, size_t n, size_t *idx, uint32_t code) {
    uint8_t *c = (uint8_t *)str + *idx;

    if(code < 0x80)
    {
        if(*idx >= n)
            return -1;
        c[0] = code;
        *idx += 1;
        return 0;
    }
    if(code < 0x07ff)
    {
        if(*idx + 1 >= n)
            return -1;
        c[0] = 0xc0 | code >> 6;
        c[1] = 0x80 | code & 0x3f;
        *idx += 2;
        return 0;
    }
    if(code < 0xffff)
    {
        if(*idx + 2 >= n)
            return -1;
        c[0] = 0xe0 | code >> 12;
        c[1] = 0x80 | (code >> 6) & 0x3f;
        c[2] = 0x80 | code & 0x3f;
        *idx += 3;
        return 0;
    }
    if(code < 0x1fffff)
    {
        if(*idx + 3 >= n)
            return -1;
        c[0] = 0xf0 | code >> 18;
        c[1] = 0x80 | (code >> 12) & 0x3f;
        c[2] = 0x80 | (code >> 6) & 0x3f;
        c[3] = 0x80 | code & 0x3f;
        *idx += 4;
        return 0;
    }
    return -1;
}

bool _unicode_is_whitespace(uint32_t code) {
    return code == 0x20 || code == '\t'; // hack
}

bool _unicode_is_word_char(uint32_t code) {
    return code > 0x20; // hack
}

int nyx_unicode_decode(const char *str, size_t n, size_t *idx, uint32_t *code) {
    return _unicode_decode(str, n, idx, code);
}

int nyx_unicode_encode(char *str, size_t n, size_t *idx, uint32_t code) {
    return _unicode_encode(str, n, idx, code);
}
