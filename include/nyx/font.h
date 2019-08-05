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

#endif
