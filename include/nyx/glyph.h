#ifndef NYX_GLYPH_H
#define NYX_GLYPH_H

int nyx_font_glyphs_import_from_bitmap_xy(const NYX_BITMAP *bitmap, uint32_t from, uint32_t to, int *x, int *y);
int nyx_font_glyphs_import_from_bitmap(const NYX_BITMAP *bitmap, int num_ranges, const uint32_t *range_pairs);
int nyx_font_glyphs_import_ascii_from_bitmap(const NYX_BITMAP *bitmap);
int nyx_font_glyphs_import(const char *path, int num_ranges, const uint32_t *range_pairs);
int nyx_font_glyphs_import_ascii(const char *path);

int nyx_glyph_exists(uint32_t code);
int nyx_glyph_width(uint32_t code);
const void *nyx_glyph_bits(uint32_t code);

int nyx_font_num_glyphs(uint32_t *num_glyphs);
int nyx_font_code_by_index(uint32_t idx, uint32_t *code);

#endif
