#include "nyx/nyx.h"

void nyx_overlap(int from1, int to1, int from2, int to2, int *fromout, int *toout) {
    *fromout = from1 > from2 ? from1 : from2;
    *toout = to1 < to2 ? to1 : to2;
}

void nyx_overlap2d(int from1x, int from1y, int to1x, int to1y, int from2x, int from2y, int to2x, int to2y, int *fromoutx, int *fromouty, int *tooutx, int *toouty) {
    nyx_overlap(from1x, to1x, from2x, to2x, fromoutx, tooutx);
    nyx_overlap(from1y, to1y, from2y, to2y, fromouty, toouty);
}
