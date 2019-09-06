#include "nyx/nyx.h"

#include "_glyph.h"

int _glyph_bit(const struct glyph *glyph, int x, int y) {
    return nyx_get_bit_unsafe(glyph->bits, x + y*glyph->w);
}

NYX_MAP *_glyphmap_init(void) {
    return nyx_init_map(4, sizeof(struct glyph));
}

uint32_t _glyphmap_size(const NYX_MAP *glyphs) {
    return nyx_map_size(glyphs);
}

struct glyph *_glyphmap_get(const NYX_MAP *glyphs, uint32_t code) {
    uint8_t key[4];

    nyx_u32_to_bytes(key, code);
    return nyx_map_get(glyphs, key);
}

int _glyphmap_add(NYX_MAP *glyphs, uint32_t code, uint16_t w, void *bits) {
    struct glyph glyph;
    uint8_t key[4];

    glyph.w = w;
    glyph.bits = bits;
    nyx_u32_to_bytes(key, code);
    return nyx_map_insert(glyphs, key, &glyph);
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
