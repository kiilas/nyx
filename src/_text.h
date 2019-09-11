#ifndef NYX__TEXT_H
#define NYX__TEXT_H

int _text_draw_char(int32_t x, int32_t y, uint32_t code, const NYX_FONT *font, NYX_COLOR color);
int _text_draw_string(const NYX_FONT *font, int32_t x, int32_t y, int wrap, bool multiline, bool nullterm, const char *str, size_t size, NYX_COLOR color);

#endif
