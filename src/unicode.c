#include "nyx/nyx.h"

int _unicode_decode(const char *str, size_t n, size_t *idx, uint32_t *code) {
    const uint8_t *c = (const uint8_t *)str + *idx;

    if(c[0] < 0x80)
    {
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

int nyx_unicode_decode(const char *str, size_t n, size_t *idx, uint32_t *code) {
    if(*idx >= n)
        return -1;
    return _unicode_decode(str, n, idx, code);
}
