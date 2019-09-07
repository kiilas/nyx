#ifndef NYX__GLYPH_H
#define NYX__GLYPH_H

struct glyph {
    uint16_t  w;
    void     *bits;
};

int _glyph_save(const struct glyph *glyph, NYX_FILE *file, int glyph_w, int glyph_h, bool monospaced);

int _glyph_bit(const struct glyph *glyph, int x, int y);

NYX_MAP *_glyphmap_init(void);

int _glyphmap_save(const NYX_MAP *glyphs, NYX_FILE *file, int glyph_w, int glyph_h, bool monospaced);

uint32_t _glyphmap_size(const NYX_MAP *glyphs);

struct glyph *_glyphmap_get(const NYX_MAP *glyphs, uint32_t code);
struct glyph *_glyphmap_by_index(const NYX_MAP *glyphs, uint32_t idx);
uint32_t _glyphmap_code_by_index(const NYX_MAP *glyphs, uint32_t idx);
int _glyphmap_add(NYX_MAP *glyphs, uint32_t key, uint16_t w, void *bits);

int _glyphmap_exists(const NYX_MAP *glyphs, uint32_t key);
int _glyphmap_width(const NYX_MAP *glyphs, uint32_t key);
const void *_glyphmap_bits(const NYX_MAP *glyphs, uint32_t key);

#endif
