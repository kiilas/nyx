#include "nyx/nyx.h"

int handle_event(NYX_EVENT *event) {
    if(event->type == NYX_EVENT_KEY_DOWN && event->keyboard.code == NYX_KEY_Q)
    {
        nyx_end();
        return 1;
    }
    return 0;
}

void render(void) {
    int w = nyx_width();
    int h = nyx_height();
    int x, y;

    for(y=0; y<h; ++y)
        for(x=0; x<w; ++x)
        {
            int ramp = x * nyx_num_ramps() / w;
            int val = y * 256 / h;
            NYX_COLOR color = {ramp, val};
            nyx_draw_pixel(x, y, color);
        }
}

int main(void) {
    if(nyx_init())
        return -1;
    nyx_set_event_handler(handle_event);
    nyx_set_renderer(render);
    nyx_start();
    return 0;
}
