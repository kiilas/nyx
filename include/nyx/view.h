#ifndef NYX_VIEW_H
#define NYX_VIEW_H

#include <stdint.h>

NYX_TRANSFORM nyx_view_transform(void);
void nyx_set_view_transform(NYX_TRANSFORM t);

int nyx_view_pan(int32_t dx, int32_t dy);
int nyx_view_pan3d(int32_t dx, int32_t dy, int32_t dz);
int nyx_view_panv(NYX_VECTOR v);

#endif
