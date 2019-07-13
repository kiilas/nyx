#include "nyx/nyx.h"

#include <math.h>
#include <stdlib.h>

#define MAX_TEXTURES 4096
#define MAX_TEXTURE_SIZE 4096

struct texture {
    uint16_t   width;
    uint16_t   height;
    NYX_TEXEL *tex;
};

static struct texture textures[MAX_TEXTURES];

static struct texture *get_texture(int id) {
    if(id < 0 || id >= MAX_TEXTURES)
        return 0;
    if(!textures[id].width || !textures[id].height)
        return 0;
    return textures + id;
}

static NYX_TEXEL get_texel(const struct texture *t, uint16_t x, uint16_t y) {
    return t->tex[x + y*t->width];
}

static void put_texel(struct texture *t, uint16_t x, uint16_t y, NYX_TEXEL texel) {
    t->tex[x + y*t->width] = texel;
}

static void render_texel(int px, int py, NYX_TEXEL texel) {
    int32_t normlen;
    float normval;
    int val;

    normlen = 1073741824 - texel.nx*texel.nx - texel.ny*texel.ny;
    if(normlen < 0)
        normlen = 0;
    normval = sqrtf(normlen / 1073741824.0);
    val = texel.value * normval * 1.0;
    if(val > 255)
        val = 255;
    NYX_COLOR c = {texel.ramp, val};
    if(!texel.trans)
        nyx_draw_pixel(px, py, c);
}

static NYX_TEXEL texel_normal(NYX_TEXEL texel, float nx, float ny) {
    NYX_TEXEL res = texel;
    res.nx = roundf(32768.0 * nx);
    res.ny = roundf(32768.0 * ny);
    return res;
}

int _render_texture(int px, int py, int id) {
    const struct texture *t = get_texture(id);

    int x, y;

    if(!t)
        return -1;
    for(y=0; y<t->height; ++y)
        for(x=0; x<t->width; ++x)
            render_texel(px+x, py+y, get_texel(t, x, y));
    return 0;
}

int nyx_make_texture(uint16_t width, uint16_t height) {
    int i;

    if(!width || !height)
        return -1;
    if(width > MAX_TEXTURE_SIZE || height > MAX_TEXTURE_SIZE)
        return -1;
    for(i=0; i<MAX_TEXTURES; ++i)
        if(!textures[i].width)
            break;
    if(i == MAX_TEXTURES)
        return -1;
    textures[i].tex = malloc(width * height * sizeof(NYX_TEXEL));
    if(!textures[i].tex)
        return -1;
    textures[i].width = width;
    textures[i].height = height;
    return i;
}

int nyx_clear_texture(int id) {
    const struct texture *t = get_texture(id);
    int i;

    if(!t)
        return -1;
    for(i=0; i<t->width*t->height; ++i)
        t->tex[i].trans = 255;
    return 0;
}

int nyx_fill_texture(int id, NYX_TEXEL texel) {
    const struct texture *t = get_texture(id);
    int i;

    if(!t)
        return -1;
    for(i=0; i<t->width*t->height; ++i)
        t->tex[i] = texel;
    return 0;
}

int nyx_texture_sphere(int id, float x, float y, float r, NYX_TEXEL texel) {
    int cx;
    int cy;

    struct texture *t = get_texture(id);
    if(!t)
        return -1;
    for(cx=floorf(x); cx<ceilf(x+r); ++cx)
        for(cy=floorf(y); cy<ceilf(y+r); ++cy)
        {
            float u = 2.0f * (cx + 0.5f - x) / r - 1.0f;
            float v = 2.0f * (cy + 0.5f - y) / r - 1.0f;

            float f = 1.0f - u*u - v*v;

            if(f >= 0)
            {
                NYX_TEXEL tn = texel_normal(texel, u, v);
                put_texel(t, cx, cy, tn);
            }
        }
    return 0;
}

int nyx_texture_ellipsoid(int id, float x, float y, float w, float h, float r, NYX_TEXEL texel) {
    int cx;
    int cy;

    struct texture *t = get_texture(id);
    if(!t)
        return -1;
    for(cx=floorf(x); cx<ceilf(x+w); ++cx)
        for(cy=floorf(y); cy<ceilf(y+h); ++cy)
        {
            float u = 2.0f * (cx + 0.5f - x) / w - 1.0f;
            float v = 2.0f * (cy + 0.5f - y) / h - 1.0f;

            float s = 1.0f - u*u - v*v;
            float f = r*r*u*u*h*h + r*r*v*v*w*w + w*w*h*h*s;

            if(s > 0 && f > 0)
            {
                float nx = r*u*h / sqrt(f);
                float ny = r*v*w / sqrt(f);

                NYX_TEXEL tn = texel_normal(texel, nx, ny);
                put_texel(t, cx, cy, tn);
            }
        }
    return 0;
}
