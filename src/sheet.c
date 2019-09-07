#include "nyx/nyx.h"

#include "_mask.h"

int _sheet_next(const NYX_BITMAP *bitmap, uint32_t border_color, int *x, int *y) {
    do
        if(++*x >= nyx_get_bitmap_width(bitmap))
        {
            if(++*y >= nyx_get_bitmap_height(bitmap))
                return 1;
            *x = 0;
        }
    while(nyx_get_bitmap_pixel_unsafe(bitmap, *x, *y) == border_color ||
          *x && nyx_get_bitmap_pixel_unsafe(bitmap, *x-1, *y) != border_color ||
          *y && nyx_get_bitmap_pixel_unsafe(bitmap, *x, *y-1) != border_color);
    return 0;
}

void _sheet_mask_bits_ptr(const NYX_BITMAP *bitmap, int x, int y, int w, int h, void *bits) {
    int px;
    int py;

    for(py=0; py<h; ++py)
        for(px=0; px<w; ++px)
        {
            int alpha = nyx_get_bitmap_pixel_alpha_unsafe(bitmap, x+px, y+py);

            nyx_set_bit_unsafe(bits, px+py*w, alpha==0xff);
        }
}

void *_sheet_mask_bits(const NYX_BITMAP *bitmap, int x, int y, int w, int h) {
    void *bits = nyx_bits_alloc(w * h);

    if(!bits)
        return 0;
    _sheet_mask_bits_ptr(bitmap, x, y, w, h, bits);
    return bits;
}

void _sheet_size(const NYX_BITMAP *bitmap, uint32_t border_color, int x, int y, int *w, int *h) {
    for(*w=1; *w<nyx_get_bitmap_width(bitmap)-x; ++*w)
        if(nyx_get_bitmap_pixel_unsafe(bitmap, x+*w, y) == border_color)
            break;
    for(*h=1; *h<nyx_get_bitmap_height(bitmap)-y; ++*h)
        if(nyx_get_bitmap_pixel_unsafe(bitmap, x, y+*h) == border_color)
            break;
}

NYX_MASK *_sheet_mask(const NYX_BITMAP *bitmap, uint32_t border_color, int *x, int *y) {
    NYX_MASK *mask;
    int w;
    int h;

    if(_sheet_next(bitmap, border_color, x, y))
        return 0;
    _sheet_size(bitmap, border_color, *x, *y, &w, &h);
    mask = nyx_mask_init(w, h);
    if(!mask)
        return 0;
    _sheet_mask_bits_ptr(bitmap, *x, *y, w, h, mask->bits);
    *x += w;
    return mask;
}
