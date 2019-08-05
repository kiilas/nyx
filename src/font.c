#include "nyx/nyx.h"

#include "_const.h"

#include "SDL2/SDL_image.h"

#include <stdio.h>

#define MAX_FONTS 256

struct kerning_key {
    uint32_t left;
    uint32_t right;
};

struct kerning_data {
    int16_t  spacing;
};

struct glyph {
    uint16_t  w;
    void     *bitfield;
};

struct NYX_FONT {
    uint16_t glyph_w;
    uint16_t glyph_h;
    int16_t  h_spacing;
    int16_t  v_spacing;
    bool     monospaced;
    bool     kerning;
    NYX_MAP *glyphs;
    NYX_MAP *kernings;
};

static NYX_FONT *fonts[MAX_FONTS];

static float default_h_spacing = DEFAULT_FONT_H_SPACING;
static float default_v_spacing = DEFAULT_FONT_V_SPACING;

static void destroy_font(NYX_FONT *f) {
    if(!f)
        return;
    nyx_destroy_map(f->glyphs);
    nyx_destroy_map(f->kernings);
}

static NYX_FONT *init_font(void) {
    NYX_FONT *f;

    f = calloc(1, sizeof(NYX_FONT));
    if(!f)
        return 0;
    f->glyphs = nyx_init_map(4, sizeof(struct glyph));
    if(!f->glyphs)
        goto fail;
    f->kernings = nyx_init_map(sizeof(struct kerning_key),
                               sizeof(struct kerning_data));
    if(!f->kernings)
        goto fail;
    return f;
fail:
    destroy_font(f);
    return 0;
}

static int next_glyph(const NYX_BITMAP *bitmap, uint32_t border_color, int *x, int *y) {
    do
        if(++*x == nyx_get_bitmap_width(bitmap))
        {
            if(++*y == nyx_get_bitmap_height(bitmap))
                return 1;
            *x = 0;
        }
    while(nyx_get_bitmap_pixel_unsafe(bitmap, *x, *y) == border_color ||
          *x && nyx_get_bitmap_pixel_unsafe(bitmap, *x-1, *y) != border_color ||
          *y && nyx_get_bitmap_pixel_unsafe(bitmap, *x, *y-1) != border_color);
    return 0;
}

static int add_glyph_data(NYX_FONT *f, uint32_t code, int w, int h, void *bitfield) {
    struct glyph glyph;

    if(!f)
        return -1;
    if(h != f->glyph_h && f->glyph_h)
        return -1;
    f->glyph_h = h;
    if(f->monospaced)
    {
        if(w != f->glyph_w && f->glyph_w)
            return -1;
        f->glyph_w = w;
    }
    glyph.w = w;
    glyph.bitfield = bitfield;
    // do we care about endianness?
    if(nyx_map_insert(f->glyphs, &code, &glyph))
        return -1;
    return 0;
}

static void *glyph_bitfield(const NYX_BITMAP *bitmap, int x, int y, int w, int h) {
    void *bitfield;
    int cx;
    int cy;

    bitfield = malloc((w*h + 7) / 8);
    if(!bitfield)
        return 0;
    for(cy=0; cy<h; ++cy)
        for(cx=0; cx<w; ++cx)
        {
            bool value = nyx_get_bitmap_pixel_alpha_unsafe(bitmap, x+cx, y+cy) == 0xff;

            nyx_set_bit_unsafe(bitfield, cx+cy*w, value);
        }
    return bitfield;
}

static int add_glyph_rect(NYX_FONT *f, const NYX_BITMAP *bitmap, int x, int y, int w, int h, uint32_t code) {
    void *bitfield;
    int res;

    if(!f)
        return -1;
    if(!bitmap)
        return -1;
    bitfield = glyph_bitfield(bitmap, x, y, w, h);
    if(!bitfield)
        return -1;
    res = add_glyph_data(f, code, w, h, bitfield);
    if(res)
        free(bitfield);
    return res;
}

static int add_glyph(NYX_FONT *f, const NYX_BITMAP *bitmap, uint32_t border_color, int x, int y, uint32_t code) {
    int w = 1;
    int h = 1;
    int px;
    int py;

    if(!f)
        return -1;
    if(!bitmap)
        return -1;
    for(px=x; px<nyx_get_bitmap_width(bitmap); ++px)
        if(nyx_get_bitmap_pixel_unsafe(bitmap, px, y) == border_color)
            break;
    w = px - x;
    if(w <= 0)
        return -1;
    for(py=y; py<nyx_get_bitmap_height(bitmap); ++py)
        if(nyx_get_bitmap_pixel_unsafe(bitmap, x, py) == border_color)
            break;
    h = py - y;
    if(h <= 0)
        return -1;
    return add_glyph_rect(f, bitmap, x, y, w, h, code);
}

static int add_glyphs(NYX_FONT *f, const NYX_BITMAP *bitmap, uint32_t from) {
    uint32_t border_color;
    uint32_t code = from;
    int x;
    int y;

    if(!f)
        return -1;
    if(nyx_get_bitmap_pixel(bitmap, 0, 0, &border_color))
        return -1;
    x = 0;
    y = 0;
    while(!next_glyph(bitmap, border_color, &x, &y))
        if(add_glyph(f, bitmap, border_color, x, y, code++))
            return -1;
    return code - from;
}

static void set_default_spacing(NYX_FONT *f) {
    f->h_spacing = 1 + f->glyph_h * default_h_spacing;
    f->v_spacing = 1 + f->glyph_h * default_v_spacing;
}

static NYX_FONT *from_bitmap(const NYX_BITMAP *bitmap) {
    NYX_FONT *f;

    f = init_font();
    if(!f)
        return 0;
    if(add_glyphs(f, bitmap, 0x20) < 0)
    {
        destroy_font(f);
        return 0;
    }
    set_default_spacing(f);
    return f;
}

int nyx_register_font(NYX_FONT *f) {
    int idx;

    for(idx=0; idx<MAX_FONTS; ++idx)
        if(!fonts[idx])
            break;
    if(idx == MAX_FONTS)
        return -1;
    fonts[idx] = f;
    return idx;
}

int nyx_make_font(void) {
    NYX_FONT *f;

    f = init_font();
    if(!f)
        return -1;
    return nyx_register_font(f);
}

int nyx_import_font(const char *path) {
    NYX_BITMAP *bitmap;
    int idx;

    bitmap = nyx_load_bitmap(path);
    if(!bitmap)
        return -1;
    idx = nyx_import_font_from_bitmap(bitmap);
    nyx_destroy_bitmap(bitmap);
    return idx;
}

int nyx_import_font_from_bitmap(const NYX_BITMAP *bitmap) {
    NYX_FONT *f;

    f = from_bitmap(bitmap);
    if(!f)
        return -1;
    return nyx_register_font(f);
}
