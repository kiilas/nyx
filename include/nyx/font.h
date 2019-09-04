#ifndef NYX_FONT_H
#define NYX_FONT_H

typedef struct NYX_FONT NYX_FONT;

typedef struct NYX_KERNING_PAIR NYX_KERNING_PAIR;

int nyx_register_font(NYX_FONT *f);

int nyx_make_font(void);

int nyx_kill_font(int idx);

int nyx_font_load(const char *path);
int nyx_font_save(const char *path);

int nyx_import_font(const char *path);
int nyx_import_font_from_bitmap(const NYX_BITMAP *bitmap);

int nyx_select_font(int idx);
int nyx_active_font(void);

int nyx_font_height(void);
int nyx_font_h_spacing(void);
int nyx_font_v_spacing(void);

int nyx_set_font_h_spacing(int16_t h_spacing);
int nyx_set_font_v_spacing(int16_t v_spacing);
int nyx_set_font_spacing(int16_t spacing);

uint32_t nyx_replacement_glyph(void);
int nyx_set_replacement_glyph(uint32_t code);

int nyx_glyph_exists(uint32_t code);
int nyx_glyph_width(uint32_t code);
const void *nyx_glyph_bits(uint32_t code);

int nyx_font_num_glyphs(uint32_t *num_glyphs);
int nyx_font_code_by_index(uint32_t idx, uint32_t *code);

int nyx_font_monospaced(void);

int nyx_font_kerning(void);
int nyx_font_set_kerning(int kerning);
int nyx_font_kerning_num_pairs(size_t *num_pairs);
int nyx_font_kerning_pair(uint32_t prev, uint32_t next);
int nyx_font_kerning_pair_set(uint32_t prev, uint32_t next, int16_t offset);
int nyx_font_kerning_pair_by_index(uint64_t index, NYX_KERNING_PAIR *pair);

int nyx_font_kerning_auto_pair(uint32_t prev, uint32_t next);
int nyx_font_kerning_auto_range(uint32_t prev_min, uint32_t prev_max, uint32_t next_min, uint32_t next_max);
int nyx_font_kerning_auto_ranges(int num_ranges, const uint32_t *range_pairs);

#endif
