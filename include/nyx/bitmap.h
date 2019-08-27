#ifndef NYX_BITMAP_H
#define NYX_BITMAP_H

typedef struct NYX_BITMAP NYX_BITMAP;

NYX_BITMAP *nyx_create_bitmap(int w, int h);
void nyx_destroy_bitmap(NYX_BITMAP *bitmap);

NYX_BITMAP *nyx_load_bitmap(const char *path);

int nyx_get_bitmap_width(const NYX_BITMAP *bitmap);
int nyx_get_bitmap_height(const NYX_BITMAP *bitmap);

uint32_t nyx_get_bitmap_pixel_unsafe(const NYX_BITMAP *bitmap, int x, int y);
int nyx_get_bitmap_pixel(const NYX_BITMAP *bitmap, int x, int y, uint32_t *color);
uint8_t nyx_get_bitmap_pixel_alpha_unsafe(const NYX_BITMAP *bitmap, int x, int y);

#endif
