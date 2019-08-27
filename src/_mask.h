#ifndef NYX__MASK_H
#define NYX__MASK_H

struct NYX_MASK {
    int   w;
    int   h;
    void *bits;
};

int _render_mask(int px, int py, NYX_MASK m, NYX_COLOR color);

#endif
