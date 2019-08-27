#include "nyx/nyx.h"

#include <stdio.h>

const char *font_path = "../../fonts/basic.png";
const char *text = "The quick dark elf jumps over the lazy orc";

const NYX_COLOR white = {0, 255};
const NYX_COLOR purple = {1, 255};

int font_id;

int handle_event(NYX_EVENT *event) {
    if(event->type == NYX_EVENT_KEY_DOWN && event->keyboard.code == NYX_KEY_Q)
    {
        nyx_end();
        return 1;
    }
    return 0;
}

void render_table() {
    int i;

    for(i=32; i<127; ++i)
    {
        int ramp = i % 16 % nyx_num_ramps();
        int val = i * 2;
        NYX_COLOR color = {ramp, val};

        nyx_draw_char(160 + i%16*8, 64 + i/16*8, i, color);
    }
}

void render_text() {
    uint64_t tic = nyx_tic();
    int ramp = tic / 512 % nyx_num_ramps();
    int brightness = tic % 512;
    NYX_COLOR color;

    if(brightness > 255)
        brightness = 511 - brightness;
    color = (NYX_COLOR){ramp, brightness};
    nyx_draw_cstring(32, 32, text, 256, color);
}

void render(void) {
    nyx_clear();
    nyx_draw_char(100, 100, 'A', white);
    nyx_draw_unichar(100, 150, 0x00000042, purple);
    render_table();
    render_text();
}

int main(void) {
    if(nyx_init())
        return -1;
    font_id = nyx_import_font(font_path);
    if(font_id < 0)
    {
        printf("couldn't load font from: %s\n", font_path);
        return -1;
    }
    nyx_select_font(font_id);
    nyx_set_event_handler(handle_event);
    nyx_set_renderer(render);
    nyx_start();
    return 0;
}
