#ifndef NYX_GEOM_H
#define NYX_GEOM_H

struct NYX_RECT {
    int x;
    int y;
    int w;
    int h;
};

typedef struct NYX_RECT NYX_RECT;

void nyx_overlap(int from1, int to1, int from2, int to2, int *fromout, int *toout);
void nyx_overlap2d(int from1x, int from1y, int to1x, int to1y, int from2x, int from2y, int to2x, int to2y, int *fromoutx, int *fromouty, int *tooutx, int *toouty);

#endif
