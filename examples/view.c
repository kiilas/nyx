#include "nyx/nyx.h"

#include <stdio.h>

#define WIDTH  640
#define HEIGHT 480
#define MARGIN 8

#define SCROLL_SPEED 2

size_t text_size;
char *text;

int font_id;
NYX_COLOR bg_color;
NYX_COLOR text_color;

int load(const char *path) {
    FILE *f = fopen(path, "r");
    if(!f)
        return -1;
    fseek(f, 0, SEEK_END);
    text_size = ftell(f);
    rewind(f);
    text = malloc(text_size);
    if(!text)
        return -1;
    fread(text, 1, text_size, f);
    fclose(f);
    return 0;
}

void randomise_colors(void) {
    bg_color = nyx_random_color();
    text_color = nyx_random_color();
}

void tic(void) {
    if(nyx_key_pressed(NYX_KEY_UP))
        nyx_view_pan(0, -SCROLL_SPEED);
    if(nyx_key_pressed(NYX_KEY_DOWN))
        nyx_view_pan(0, SCROLL_SPEED);
}

int handle_event(NYX_EVENT *event) {
    if(event->type == NYX_EVENT_KEY_DOWN)
    {
        if(event->keyboard.code == NYX_KEY_Q)
        {
            nyx_end();
            return 1;
        }
        if(event->keyboard.code == NYX_KEY_R)
        {
            randomise_colors();
            return 1;
        }
        if(event->keyboard.code == NYX_KEY_PGUP)
        {
            nyx_view_pan(0, -HEIGHT);
            return 1;
        }
        if(event->keyboard.code == NYX_KEY_PGDOWN)
        {
            nyx_view_pan(0, HEIGHT);
            return 1;
        }
    }
    return 0;
}

void render() {
    nyx_fill(bg_color);
    nyx_draw_cstring_multiline(MARGIN,
                               MARGIN,
                               WIDTH - 2*MARGIN,
                               text,
                               text_size,
                               text_color);
}

int main(int argc, char **argv) {
    if(argc != 3)
    {
        printf("view FONT TEXTFILE\n");
        return 2;
    }
    if(load(argv[2]))
        return -1;
    nyx_set_resolution(WIDTH, HEIGHT);
    if(nyx_init())
        return -1;
    font_id = nyx_font_load(argv[1]);
    if(font_id < 0)
        return -1;
    nyx_select_font(font_id);
    randomise_colors();
    nyx_set_tic_handler(tic);
    nyx_set_event_handler(handle_event);
    nyx_set_renderer(render);
    nyx_start();
    return 0;
}
