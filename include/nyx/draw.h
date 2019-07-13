#ifndef NYX_DRAW_H
#define NYX_DRAW_H

#include "nyx/linear.h"

void nyx_clear(void);
void nyx_fill(NYX_COLOR color);
int nyx_draw_pixel(int32_t x, int32_t y, NYX_COLOR color);
int nyx_draw_pixel3d(int32_t x, int32_t y, int32_t z, NYX_COLOR color);
int nyx_draw_pixelv(NYX_VECTOR v, NYX_COLOR color);
int nyx_draw_rect(int32_t x, int32_t y, int32_t w, int32_t h, NYX_COLOR color);
int nyx_draw_rect3d(int32_t x, int32_t y, int32_t z, int32_t w, int32_t h, NYX_COLOR color);
int nyx_draw_rectv(NYX_VECTOR v, int32_t w, int32_t h, NYX_COLOR color);
int nyx_draw_texture(int32_t x, int32_t y, int id);
int nyx_draw_texturev(NYX_VECTOR v, int id);

#endif
