#ifndef NYX__GLYPH_H
#define NYX__GLYPH_H

struct glyph {
    uint16_t  w;
    void     *bits;
};

int _glyph_bit(const struct glyph *glyph, int x, int y);

NYX_MAP *_glyphmap_init(void);

uint32_t _glyphmap_size(const NYX_MAP *glyphs);

struct glyph *_glyphmap_get(const NYX_MAP *glyphs, uint32_t code);
int _glyphmap_add(NYX_MAP *glyphs, uint32_t key, uint16_t w, void *bits);

int _glyphmap_exists(const NYX_MAP *glyphs, uint32_t key);
int _glyphmap_width(const NYX_MAP *glyphs, uint32_t key);
const void *_glyphmap_bits(const NYX_MAP *glyphs, uint32_t key);

#endif
