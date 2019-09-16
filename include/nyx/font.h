#ifndef NYX_FONT_H
#define NYX_FONT_H

typedef struct NYX_FONT NYX_FONT;

int nyx_make_font(void);

int nyx_kill_font(int idx);

int nyx_font_load(const char *path);
int nyx_font_save(const char *path);

int nyx_font_import_from_bitmap(const NYX_BITMAP *bitmap, int num_ranges, const uint32_t *range_pairs);
int nyx_font_import_ascii_from_bitmap(const NYX_BITMAP *bitmap);
int nyx_font_import(const char *path, int num_ranges, uint32_t *range_pairs);
int nyx_font_import_ascii(const char *path);

int nyx_select_font(int idx);
int nyx_active_font(void);

int nyx_font_height(void);
int nyx_font_h_spacing(void);
int nyx_font_v_spacing(void);
int nyx_set_font_h_spacing(int16_t h_spacing);
int nyx_set_font_v_spacing(int16_t v_spacing);
int nyx_set_font_spacing(int16_t spacing);
int nyx_set_font_spacing_auto(void);

uint32_t nyx_replacement_glyph(void);
int nyx_set_replacement_glyph(uint32_t code);

int nyx_font_monospaced(void);

#endif
