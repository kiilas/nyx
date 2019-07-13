#ifndef NYX_LAYER_H
#define NYX_LAYER_H

#include "nyx/transform.h"

int nyx_make_layer(void);

int nyx_select_layer(int layer_num);

NYX_TRANSFORM nyx_layer_transform(void);
int nyx_set_layer_transform(NYX_TRANSFORM t);
NYX_TRANSFORM nyx_default_layer_transform(void);
void nyx_set_default_layer_transform(NYX_TRANSFORM t);

#endif
