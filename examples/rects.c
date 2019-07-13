#include "nyx/nyx.h"

#define NUM_RECTS 256

struct rect {
    float x, y;
    float w, h;
    float vx, vy;
    NYX_COLOR color;
} rects[NUM_RECTS];

void tic(void) {
    int i;

    for(i=0; i<NUM_RECTS; ++i)
    {
        struct rect *r = rects + i;

        r->x += r->vx;
        r->y += r->vy;
        if(r->x < -r->w)
            r->x += nyx_width() + r->w;
        else if(r->x > nyx_width())
            r->x -= nyx_width() + r->w;
        if(r->y < -r->h)
            r->y += nyx_height() + r->h;
        else if(r->y > nyx_height())
            r->y -= nyx_height() + r->h;
    }
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

    nyx_clear();
    for(i=0; i<NUM_RECTS; ++i)
    {
        const struct rect *r = rects + i;

        nyx_draw_rect(r->x, r->y, r->w, r->h, r->color);
    }
}

void init_rects(void) {
    int i;

    for(i=0; i<NUM_RECTS; ++i)
    {
        struct rect *r = rects + i;

        r->x = nyx_random_float_range(-r->w, nyx_width());
        r->y = nyx_random_float_range(-r->h, nyx_height());
        r->w = nyx_random_float_range(8, 64);
        r->h = nyx_random_float_range(8, 32);
        r->vx = nyx_random_float_range(-2, 2);
        r->vy = nyx_random_float_range(-2, 2);
        r->color = nyx_random_color();
        if(r->color.value < 32)
            r->color.value = 32;
    }
}

int main(void) {
    if(nyx_init())
        return -1;
    init_rects();
    nyx_set_tic_handler(tic);
    nyx_set_event_handler(handle_event);
    nyx_set_renderer(render);
    nyx_start();
    return 0;
}
