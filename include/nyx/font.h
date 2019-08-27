#ifndef NYX_FONT_H
#define NYX_FONT_H

typedef struct NYX_FONT NYX_FONT;

int nyx_register_font(NYX_FONT *f);

int nyx_make_font(void);

int nyx_kill_font(int idx);

int nyx_load_font(const char *path);
int nyx_save_font(const char *path);

int nyx_import_font(const char *path);
int nyx_import_font_from_bitmap(const NYX_BITMAP *bitmap);

int nyx_select_font(int idx);
int nyx_active_font(void);

int nyx_font_height(void);
int nyx_font_h_spacing(void);

int nyx_glyph_width(uint32_t code);
const void *nyx_glyph_bits(uint32_t code);

#endif
