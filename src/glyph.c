#include "nyx/nyx.h"

#include "_glyph.h"

struct glyph *_glyph_init(int width, int height) {
    struct glyph *glyph = calloc(1, sizeof(struct glyph));

    if(!glyph)
        return 0;
    if(width)
    {
        glyph->bits = nyx_bits_alloc(width * height);
        if(!glyph->bits)
            goto fail;
        glyph->w = width;
    }
    return glyph;
fail:
    free(glyph);
    return 0;
}

void _glyph_destroy(struct glyph *glyph) {
    if(glyph->w)
        free(glyph->bits);
    free(glyph);
}

struct glyph *_glyph_load(NYX_FILE *file, int glyph_w, int glyph_h, bool monospaced) {
    struct glyph *glyph;
    int64_t width;

    if(monospaced)
        width = glyph_w;
    else if(nyx_file_read_upak(file, &width))
        return 0;
    glyph = _glyph_init(width, glyph_h);
    if(!glyph)
        return 0;
    if(width && nyx_file_read_bits(file, glyph->bits, width * glyph_h))
        goto fail;
    return glyph;
fail:
    _glyph_destroy(glyph);
    return 0;
}

int _glyph_save(const struct glyph *glyph, NYX_FILE *file, int glyph_w, int glyph_h, bool monospaced) {
    int width = monospaced ? glyph_w : glyph->w;

    if(!monospaced && nyx_file_write_upak(file, width) ||
       nyx_file_write_bits(file, glyph->bits, width * glyph_h))
        return -1;
    return 0;
}

int _glyph_bit(const struct glyph *glyph, int x, int y) {
    return nyx_get_bit_unsafe(glyph->bits, x + y*glyph->w);
}

NYX_MAP *_glyphmap_init(void) {
    return nyx_init_map(4, sizeof(struct glyph));
}

int _glyphmap_load(NYX_MAP *glyphs, NYX_FILE *file, int glyph_w, int glyph_h, bool monospaced) {
    int64_t size;
    uint32_t idx;
    uint32_t previous;

    if(nyx_file_read_upak(file, &size))
        return -1;
    for(idx=0; idx<size; ++idx)
    {
        struct glyph *glyph;
        int64_t offset;
        uint32_t code;

        if(nyx_file_read_upak(file, &offset))
            return -1;
        glyph = _glyph_load(file, glyph_w, glyph_h, monospaced);
        if(!glyph)
            return -1;
        code = idx ? previous+offset+1 : offset;
        previous = code;
        if(_glyphmap_add(glyphs, code, glyph))
        {
            _glyph_destroy(glyph);
            return -1;
        }
    }
    return 0;
}

int _glyphmap_save(const NYX_MAP *glyphs, NYX_FILE *file, int glyph_w, int glyph_h, bool monospaced) {
    uint32_t size = _glyphmap_size(glyphs);
    uint32_t idx;
    uint32_t previous;

    if(nyx_file_write_upak(file, size))
        return -1;
    for(idx=0; idx<size; ++idx)
    {
        const struct glyph *glyph = _glyphmap_by_index(glyphs, idx);
        uint32_t code = _glyphmap_code_by_index(glyphs, idx);
        uint32_t packed_prev;

        packed_prev = idx ? code-previous-1 : code;
        if(nyx_file_write_upak(file, packed_prev))
            return -1;
        if(_glyph_save(glyph, file, glyph_w, glyph_h, monospaced))
            return -1;
        previous = code;
    }
    return 0;
}

uint32_t _glyphmap_size(const NYX_MAP *glyphs) {
    return nyx_map_size(glyphs);
}

struct glyph *_glyphmap_get(const NYX_MAP *glyphs, uint32_t code) {
    uint8_t key[4];

    nyx_u32_to_bytes(key, code);
    return nyx_map_get(glyphs, key);
}

struct glyph *_glyphmap_by_index(const NYX_MAP *glyphs, uint32_t idx) {
    return nyx_map_value_by_index(glyphs, idx);
}

uint32_t _glyphmap_code_by_index(const NYX_MAP *glyphs, uint32_t idx) {
    const void *key;

    key = nyx_map_key_by_index(glyphs, idx);
    return nyx_bytes_to_u32(key);
}

int _glyphmap_add(NYX_MAP *glyphs, uint32_t code, struct glyph *glyph) {
    uint8_t key[4];

    nyx_u32_to_bytes(key, code);
    return nyx_map_insert(glyphs, key, glyph);
}

int _glyphmap_exists(const NYX_MAP *glyphs, uint32_t code) {
    return _glyphmap_get(glyphs, code) != 0;
}

int _glyphmap_width(const NYX_MAP *glyphs, uint32_t code) {
    const struct glyph *g = _glyphmap_get(glyphs, code);

    return g ? g->w : -1;
}

const void *_glyphmap_bits(const NYX_MAP *glyphs, uint32_t code) {
    const struct glyph *g = _glyphmap_get(glyphs, code);

    return g ? g->bits : 0;
}
