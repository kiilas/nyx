#include "nyx/nyx.h"

#include "_graphics.h"
#include "_mask.h"

int _render_mask(int px, int py, NYX_MASK m, NYX_COLOR color) {
    int x1, y1, x2, y2;
    int x, y;

    nyx_screen_overlap(px, py, px+m.w, py+m.h, &x1, &y1, &x2, &y2);
    for(y=y1; y<y2; ++y)
        for(x=x1; x<x2; ++x)
        {
            int mx = x - px;
            int my = y - py;
            int mbit = mx + my*m.w;

            if(nyx_get_bit_unsafe(m.bits, mbit))
                _set_pixel_unsafe(x, y, color);
        }
    return 0;
}
