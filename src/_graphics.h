#ifndef NYX__GRAPHICS_H
#define NYX__GRAPHICS_H

int _init_graphics(void);
int _init_display(void);
int _init_palette(void);

int _lock_screen(void);
void _unlock_screen(void);
int _flip(void);

uint32_t *_pixel(int x, int y);

uint32_t _color(NYX_COLOR color, uint8_t x, uint8_t y);

void _set_pixel_unsafe(int x, int y, NYX_COLOR color);
void _set_pixel(int x, int y, NYX_COLOR color);
void _set_rect_unsafe(int x, int y, int w, int h, NYX_COLOR color);
void _set_rect(int x, int y, int w, int h, NYX_COLOR color);

#endif
