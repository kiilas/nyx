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

NYX_MASK *nyx_mask_init(int w, int h) {
    NYX_MASK *mask = malloc(sizeof(NYX_MASK));

    mask->bits = nyx_bits_alloc(w * h);
    if(!mask->bits)
    {
        free(mask);
        return 0;
    }
    mask->w = w;
    mask->h = h;
    return mask;
}

void nyx_mask_destroy(NYX_MASK *mask) {
    if(!mask)
        return;
    free(mask->bits);
    free(mask);
}

void *nyx_mask_bits_copy(const NYX_MASK *mask) {
    size_t size = (mask->w*mask->h + 7) / 8;
    void *bits = malloc(size);
    
    if(!bits)
        return 0;
    memcpy(bits, mask->bits, size);
    return bits;
}
