#include "nyx/nyx.h"

#include "_pipeline.h"

#define MAX_LAYERS 256

#define LAYER_DEFS {.active=true, .transform={.type=NYX_TRANSFORM_TYPE_NONE}}

struct layer {
    bool active;
    NYX_TRANSFORM transform;
};

static struct layer layers[MAX_LAYERS];
static struct layer new_layer_defs = LAYER_DEFS;

static int active_layer;

static struct layer *get_layer(int layer_num) {
    if(active_layer < 0 || layer_num >= MAX_LAYERS)
        return 0;
    if(!layers[layer_num].active)
        return 0;
    return layers + layer_num;
}

static struct layer *get_active_layer(void) {
    return get_layer(active_layer);
}

static NYX_TRANSFORM *active_transform_ptr(void) {
    struct layer *l = get_active_layer();

    return l ? &l->transform : 0;
}

NYX_VECTOR _layer_transform(NYX_VECTOR v) {
    return nyx_transform(v, active_transform_ptr());
}

int nyx_make_layer(void) {
    int i;

    for(i=0; i<MAX_LAYERS; ++i)
        if(!layers[i].active)
            break;
    if(i == MAX_LAYERS)
        return -1;
    layers[i] = new_layer_defs;
    return i;
}

int nyx_select_layer(int layer_num) {
    if(layer_num < 0 || layer_num >= MAX_LAYERS)
        return -1;
    if(!layers[layer_num].active)
        return -1;
    active_layer = layer_num;
    _update_pipeline();
    return 0;
}

NYX_TRANSFORM nyx_layer_transform(void) {
    const NYX_TRANSFORM *t = active_transform_ptr();

    return t ? *t : NYX_TRANSFORM_IDENTITY;
}

int nyx_set_layer_transform(NYX_TRANSFORM t) {
    struct layer *l = get_active_layer();

    if(!l)
        return -1;
    l->transform = t;
    _update_pipeline();
    return 0;
}

NYX_TRANSFORM nyx_default_layer_transform(void) {
    return new_layer_defs.transform;
}

void nyx_set_default_layer_transform(NYX_TRANSFORM t) {
    new_layer_defs.transform = t;
}
