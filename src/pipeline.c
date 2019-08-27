#include "nyx/nyx.h"

#include "_graphics.h"
#include "_mask.h"
#include "_texture.h"

#define PIPELINE_LENGTH 4

uint8_t pipeline_length;
NYX_TRANSFORM pipeline[PIPELINE_LENGTH];

static int pipeline_add(NYX_TRANSFORM t) {
    if(pipeline_length == PIPELINE_LENGTH)
        return -1;
    switch(t.type) {
        case NYX_TRANSFORM_TYPE_INBOUNDS:
        case NYX_TRANSFORM_TYPE_NONE:
            return 0;
        default:
            pipeline[pipeline_length++] = t;
            return 0;
    }
}

static int pipeline_pixel(NYX_VECTOR v, NYX_COLOR color, int8_t idx) {
    NYX_VECTOR v_new;

    if(idx == pipeline_length)
    {
        _set_pixel(v.a[0], v.a[1], color);
        return 0;
    }
    v_new = nyx_transform(v, pipeline+idx);
    return pipeline_pixel(v_new, color, idx+1);
}

static int pipeline_rect(NYX_VECTOR v, NYX_VECTOR size, NYX_COLOR color, uint8_t idx) {
    if(idx == pipeline_length)
    {
        _set_rect(v.a[0], v.a[1], size.a[0], size.a[1], color);
        return 0;
    }
    switch(pipeline[idx].type) {
        NYX_VECTOR v_new;
        NYX_VECTOR size_new;

        case NYX_TRANSFORM_TYPE_INBOUNDS:
        case NYX_TRANSFORM_TYPE_NONE:
        case NYX_TRANSFORM_TYPE_TRANS:
        case NYX_TRANSFORM_TYPE_TRANS_ISCALE:
        case NYX_TRANSFORM_TYPE_TRANS_SCALE:
            v_new = nyx_transform(v, pipeline+idx);
            size_new = nyx_transform_scale(size, pipeline+idx);
            return pipeline_rect(v_new, size_new, color, idx+1);
        case NYX_TRANSFORM_TYPE_SIMPLE_CAMERA:
            v_new = nyx_transform(v, pipeline+idx);
            if(!v_new.a[2])
                return -1;
            size_new = nyx_vector_scale(size, nyx_scale((float)pipeline[idx].simple_camera.focus/v_new.a[2], (float)pipeline[idx].simple_camera.focus/v_new.a[2], 1.0f));
            return pipeline_rect(v_new, size_new, color, idx+1);
    }
    return -1;
}

static int pipeline_texture(NYX_VECTOR v, int id, uint8_t idx) {
    if(idx == pipeline_length)
        return _render_texture(v.a[0], v.a[1], id);
    switch(pipeline[idx].type) {
        NYX_VECTOR v_new;

        case NYX_TRANSFORM_TYPE_INBOUNDS:
        case NYX_TRANSFORM_TYPE_NONE:
        case NYX_TRANSFORM_TYPE_TRANS:
            v_new = nyx_transform(v, pipeline+idx);
            return pipeline_texture(v_new, id, idx+1);
    }
    return -1;
}

static int pipeline_mask(NYX_VECTOR v, NYX_MASK m, NYX_COLOR color, uint8_t idx) {
    if(idx == pipeline_length)
        return _render_mask(v.a[0], v.a[1], m, color);
    switch(pipeline[idx].type) {
        NYX_VECTOR v_new;

        case NYX_TRANSFORM_TYPE_INBOUNDS:
        case NYX_TRANSFORM_TYPE_NONE:
        case NYX_TRANSFORM_TYPE_TRANS:
            v_new = nyx_transform(v, pipeline+idx);
            return pipeline_mask(v_new, m, color, idx+1);
    }
    return -1;
}

void _update_pipeline(void) {
    pipeline_length = 0;

    pipeline_add(nyx_layer_transform());
    pipeline_add(nyx_view_transform());
}

int _pipeline_pixel(NYX_VECTOR v, NYX_COLOR color) {
    return pipeline_pixel(v, color, 0);
}

int _pipeline_rect(NYX_VECTOR v, int32_t w, int32_t h, NYX_COLOR color) {
    NYX_VECTOR size = nyx_vector(w, h);

    return pipeline_rect(v, size, color, 0);
}

int _pipeline_texture(NYX_VECTOR v, int id) {
    return pipeline_texture(v, id, 0);
}

int _pipeline_mask(NYX_VECTOR v, NYX_MASK m, NYX_COLOR color) {
    return pipeline_mask(v, m, color, 0);
}
