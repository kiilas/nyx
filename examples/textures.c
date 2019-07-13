#include "nyx/nyx.h"

#define BACKGROUND (NYX_COLOR){.ramp=1, .value=64}

#define NUM_TEXTURES 64
#define TEXTURE_SIZE 32

int ids[NUM_TEXTURES];
NYX_VECTOR positions[NUM_TEXTURES];

int init_textures(void) {
    int i;

    for(i=0; i<NUM_TEXTURES; ++i)
    {
        int w = nyx_random_float_range(TEXTURE_SIZE/2, TEXTURE_SIZE*2);
        int h = nyx_random_float_range(TEXTURE_SIZE/2, TEXTURE_SIZE*2);
        int d = nyx_random_float_range(TEXTURE_SIZE/2, TEXTURE_SIZE*2);
        int x = nyx_random_range(-w+1, nyx_width()-1);
        int y = nyx_random_range(-h+1, nyx_height()-1);
        NYX_COLOR color = nyx_random_color();
        NYX_TEXEL texel = {.ramp=color.ramp, .value=color.value};

        int id = nyx_make_texture(w, h);

        if(id < 0)
            return -1;
        nyx_clear_texture(id);
        nyx_texture_ellipsoid(id, 0, 0, w, h, d, texel);
        ids[i] = id;
        positions[i] = nyx_vector(x, y);
    }
    return 0;
}

void tic(void) {
    if(nyx_key_pressed(NYX_KEY_RIGHT))
        nyx_view_pan(1, 0);
    if(nyx_key_pressed(NYX_KEY_LEFT))
        nyx_view_pan(-1, 0);
    if(nyx_key_pressed(NYX_KEY_UP))
        nyx_view_pan(0, -1);
    if(nyx_key_pressed(NYX_KEY_DOWN))
        nyx_view_pan(0, 1);
    if(nyx_key_pressed(NYX_KEY_Z))
        nyx_set_dither_squish(nyx_dither_squish() - 0.01);
    if(nyx_key_pressed(NYX_KEY_X))
        nyx_set_dither_squish(nyx_dither_squish() + 0.01);
    if(nyx_key_pressed(NYX_KEY_C))
        nyx_set_dither_squish(1.0);
    if(nyx_key_pressed(NYX_KEY_V))
        nyx_set_dither_squish(0.0);
}

int handle_event(NYX_EVENT *event) {
    if(event->type == NYX_EVENT_KEY_DOWN && event->keyboard.code == NYX_KEY_Q)
    {
        nyx_end();
        return 1;
    }
    return 0;
}

void render(void) {
    int i;

    nyx_fill(BACKGROUND);
    for(i=0; i<NUM_TEXTURES; ++i)
        nyx_draw_texturev(positions[i], ids[i]);
}

int main(void) {
    if(nyx_init())
        return -1;
    nyx_set_dither_squish(1.0);
    if(init_textures())
        return -1;
    nyx_set_tic_handler(tic);
    nyx_set_event_handler(handle_event);
    nyx_set_renderer(render);
    nyx_start();
}
