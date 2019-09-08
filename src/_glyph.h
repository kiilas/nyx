#ifndef NYX__GLYPH_H
#define NYX__GLYPH_H

struct glyph {
    uint16_t  w;
    void     *bits;
};

struct glyph *_glyph_init(int width, int height);
void _glyph_destroy(struct glyph *glyph);
void _glyph_destroy_keep_bits(struct glyph *glyph);

struct glyph *_glyph_load(NYX_FILE *file, int glyph_w, int glyph_h, bool monospaced);
int _glyph_save(const struct glyph *glyph, NYX_FILE *file, int glyph_w, int glyph_h, bool monospaced);

int _glyph_bit(const struct glyph *glyph, int x, int y);

NYX_MAP *_glyphmap_init(void);

int _glyphmap_load(NYX_MAP *glyphs, NYX_FILE *file, int glyph_w, int glyph_h, bool monospaced);
int _glyphmap_save(const NYX_MAP *glyphs, NYX_FILE *file, int glyph_w, int glyph_h, bool monospaced);

uint32_t _glyphmap_size(const NYX_MAP *glyphs);

struct glyph *_glyphmap_get(const NYX_MAP *glyphs, uint32_t code);
struct glyph *_glyphmap_by_index(const NYX_MAP *glyphs, uint32_t idx);
uint32_t _glyphmap_code_by_index(const NYX_MAP *glyphs, uint32_t idx);
int _glyphmap_add_glyph(NYX_MAP *glyphs, uint32_t code, struct glyph *glyph);
int _glyphmap_add(NYX_MAP *glyphs, uint32_t code, uint16_t w, void *bits);

int _glyphmap_exists(const NYX_MAP *glyphs, uint32_t code);
int _glyphmap_width(const NYX_MAP *glyphs, uint32_t code);
const void *_glyphmap_bits(const NYX_MAP *glyphs, uint32_t code);

#endif
