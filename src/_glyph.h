#ifndef NYX__GLYPH_H
#define NYX__GLYPH_H

struct glyph {
    uint16_t  w;
    void     *bits;
};

int _glyph_bit(const struct glyph *glyph, int x, int y);

NYX_MAP *_glyphmap_init(void);

int _glyphmap_load(NYX_MAP *glyphs, NYX_FILE *file, int glyph_w, int glyph_h, bool monospaced);
int _glyphmap_save(const NYX_MAP *glyphs, NYX_FILE *file, int glyph_w, int glyph_h, bool monospaced);

struct glyph *_glyphmap_get(const NYX_MAP *glyphs, uint32_t code);

int _font_add_from_bitmap(NYX_FONT *font, const NYX_BITMAP *bitmap, int num_ranges, const uint32_t *range_pairs);

#endif
