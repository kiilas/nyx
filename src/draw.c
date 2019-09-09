#include "nyx/nyx.h"

#include "_font.h"
#include "_graphics.h"
#include "_layer.h"
#include "_pipeline.h"
#include "_text.h"
#include "_texture.h"

void nyx_clear(void) {
    nyx_fill(nyx_get_background_color());
}

void nyx_fill(NYX_COLOR color) {
    _set_rect_unsafe(0, 0, nyx_width(), nyx_height(), color);
}

int nyx_draw_pixel(int32_t x, int32_t y, NYX_COLOR color) {
    NYX_VECTOR v = nyx_vector(x, y);

    return _pipeline_pixel(v, color);
}

int nyx_draw_pixel3d(int32_t x, int32_t y, int32_t z, NYX_COLOR color) {
    NYX_VECTOR v = nyx_vector(x, y, z);

    return _pipeline_pixel(v, color);
}

int nyx_draw_pixelv(NYX_VECTOR v, NYX_COLOR color) {
    return _pipeline_pixel(v, color);
}

int nyx_draw_rect(int32_t x, int32_t y, int32_t w, int32_t h, NYX_COLOR color) {
    NYX_VECTOR v = nyx_vector(x, y);

    return _pipeline_rect(v, w, h, color);
}

int nyx_draw_rect3d(int32_t x, int32_t y, int32_t z, int32_t w, int32_t h, NYX_COLOR color) {
    NYX_VECTOR v = nyx_vector(x, y, z);

    return _pipeline_rect(v, w, h, color);
}

int nyx_draw_rectv(NYX_VECTOR v, int32_t w, int32_t h, NYX_COLOR color) {
    return _pipeline_rect(v, w, h, color);
}

int nyx_draw_texture(int32_t x, int32_t y, int id) {
    NYX_VECTOR v = nyx_vector(x, y);

    return _pipeline_texture(v, id);
}

int nyx_draw_texturev(NYX_VECTOR v, int id) {
    return _pipeline_texture(v, id);
}

int nyx_draw_char(int32_t x, int32_t y, uint32_t code, NYX_COLOR color) {
    const NYX_FONT *font = _get_active_font();

    if(!font)
        return -1;
    return _text_draw_char(x, y, code, font, color);
}

static int draw_string(int32_t x, int32_t y, int wrap, bool multiline, bool nullterm, const char *str, size_t size, NYX_COLOR color) {
    const NYX_FONT *font = _get_active_font();

    if(!font)
        return -1;
    return _text_draw_string(font, x, y, wrap, multiline,
                             nullterm, str, size, color);
}

int nyx_draw_string(int32_t x, int32_t y, const char *str, size_t size, NYX_COLOR color) {
    return draw_string(x, y, 0, false, false, str, size, color);
}

int nyx_draw_string_multiline(int32_t x, int32_t y, int wrap, const char *str, size_t size, NYX_COLOR color) {
    return draw_string(x, y, wrap, true, false, str, size, color);
}

int nyx_draw_cstring(int32_t x, int32_t y, const char *str, size_t max_size, NYX_COLOR color) {
    return draw_string(x, y, 0, false, true, str, max_size, color);
}

int nyx_draw_cstring_multiline(int32_t x, int32_t y, int wrap, const char *str, size_t max_size, NYX_COLOR color) {
    return draw_string(x, y, wrap, true, true, str, max_size, color);
}
