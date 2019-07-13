#ifndef NYX_TEXTURE_H
#define NYX_TEXTURE_H

typedef struct NYX_TEXEL {
    uint8_t ramp;
    uint8_t value;
    int16_t nx;
    int16_t ny;
    uint8_t trans;
    uint8_t light;
} NYX_TEXEL;

int nyx_make_texture(uint16_t width, uint16_t height);

int nyx_clear_texture(int id);
int nyx_fill_texture(int id, NYX_TEXEL texel);

int nyx_texture_sphere(int id, float x, float y, float r, NYX_TEXEL texel);
int nyx_texture_ellipsoid(int id, float x, float y, float w, float h, float t, NYX_TEXEL texel);

#endif
