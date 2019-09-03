#include "nyx/nyx.h"

#include "_graphics.h"
#include "_layer.h"
#include "_mask.h"
#include "_pipeline.h"
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

int nyx_draw_char(int32_t x, int32_t y, char c, NYX_COLOR color) {
    return nyx_draw_unichar(x, y, (uint8_t)c, color);
}

int nyx_draw_unichar(int32_t x, int32_t y, uint32_t code, NYX_COLOR color) {
    const void *bits = nyx_glyph_bits(code);
    int w;
    int h;
    NYX_MASK m;
    NYX_VECTOR v;

    if(nyx_active_font() < 0)
        return -1;
    // if undefined glyph, try to render replacement glyph
    if(!bits)
    {
        uint32_t replacement = nyx_replacement_glyph();
        if(code != replacement)
            return nyx_draw_unichar(x, y, replacement, color);
        return -1;
    }
    w = nyx_glyph_width(code);
    h = nyx_font_height();
    v = nyx_vector(x, y);
    m = (NYX_MASK){.w=w, .h=h, .bits=(void *)bits};
    return _pipeline_mask(v, m, color);
}

int nyx_draw_cstring(int32_t x, int32_t y, const char *str, size_t n, NYX_COLOR color) {
    size_t idx;

    for(idx=0; idx<n; ++idx)
    {
        uint32_t code = str[idx];

        if(!code)
            break;
        if(idx)
            x -= nyx_font_kerning_pair(str[idx-1], code);
        if(nyx_draw_char(x, y, code, color))
            return -1;
        x += nyx_glyph_width(code);
        x += nyx_font_h_spacing();
    }
    return 0;
}

int nyx_draw_cstring_multiline(int32_t x, int32_t y, int wrap, const char *str, size_t n, NYX_COLOR color) {
    size_t idx;
    int pos_x = x;
    int pos_y = y;

    for(idx=0; idx<n; ++idx)
    {
        uint32_t code = str[idx];
        int width;

        if(!code)
            break;
        if(code == '\n')
        {
            pos_x = x;
            pos_y += nyx_font_height();
            pos_y += nyx_font_v_spacing();
            continue;
        }
        // temporary workaround
        if(code < 0x20)
            continue;
        if(idx)
            pos_x -= nyx_font_kerning_pair(str[idx-1], code);
        width = nyx_glyph_width(code);
        if(wrap && pos_x+width>x+wrap && pos_x!=x)
        {
            pos_x = x;
            pos_y += nyx_font_height();
            pos_y += nyx_font_v_spacing();
        }
        if(nyx_draw_char(pos_x, pos_y, code, color))
            return -1;
        pos_x += width;
        pos_x += nyx_font_h_spacing();
    }
    return 0;
}
