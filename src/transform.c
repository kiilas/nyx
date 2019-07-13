#include "nyx/nyx.h"

#include <assert.h>

NYX_VECTOR nyx_transform(NYX_VECTOR v, const NYX_TRANSFORM *t) {
    if(!t)
        return v;
    switch(t->type) {
        NYX_VECTOR v_trans;

        case NYX_TRANSFORM_TYPE_INBOUNDS:
        case NYX_TRANSFORM_TYPE_NONE:
            return v;
        case NYX_TRANSFORM_TYPE_TRANS:
            return nyx_vector_add(v, t->trans.trans);
        case NYX_TRANSFORM_TYPE_TRANS_ISCALE:
            v_trans = nyx_vector_add(v, t->trans_iscale.trans);
            return nyx_vector_iscale(v_trans, t->trans_iscale.scale);
        case NYX_TRANSFORM_TYPE_TRANS_SCALE:
            v_trans = nyx_vector_add(v, t->trans_scale.trans);
            return nyx_vector_scale(v_trans, t->trans_scale.scale);
        case NYX_TRANSFORM_TYPE_AFFINE:
            return nyx_affine(v, t->affine.map);
        case NYX_TRANSFORM_TYPE_SIMPLE_CAMERA:
            v_trans = nyx_vector_add(v, t->simple_camera.trans);
            return nyx_vector_add(nyx_vector_scale(v_trans, nyx_scale((float)t->simple_camera.focus/v_trans.a[2], (float)t->simple_camera.focus/v_trans.a[2], 1.0f)), nyx_vector(t->simple_camera.w/2, t->simple_camera.h/2));
        case NYX_TRANSFORM_TYPE_FUNC:
            return t->func.f(v);
    }
    assert(0);
    return NYX_VECTOR_ZERO;
}

NYX_VECTOR nyx_transform_scale(NYX_VECTOR v, const NYX_TRANSFORM *t) {
    if(!t)
        return v;
    switch(t->type) {
        case NYX_TRANSFORM_TYPE_INBOUNDS:
        case NYX_TRANSFORM_TYPE_NONE:
        case NYX_TRANSFORM_TYPE_TRANS:
            return v;
        case NYX_TRANSFORM_TYPE_TRANS_ISCALE:
            return nyx_vector_iscale(v, t->trans_iscale.scale);
        case NYX_TRANSFORM_TYPE_TRANS_SCALE:
            return nyx_vector_scale(v, t->trans_scale.scale);
        case NYX_TRANSFORM_TYPE_AFFINE:
            return nyx_linear(v, t->affine.map.linear_map);
        case NYX_TRANSFORM_TYPE_FUNC:
            assert(0);
            return NYX_VECTOR_ZERO;
    }
    assert(0);
    return NYX_VECTOR_ZERO;
}

NYX_TRANSFORM nyx_trans(NYX_VECTOR v) {
    return (NYX_TRANSFORM){.trans={.type=NYX_TRANSFORM_TYPE_TRANS, .trans=v}};
}

NYX_TRANSFORM nyx_trans_iscale(NYX_VECTOR v, NYX_ISCALE s) {
    return (NYX_TRANSFORM){.trans_iscale={.type=NYX_TRANSFORM_TYPE_TRANS_ISCALE, .trans=v, .scale=s}};
}

NYX_TRANSFORM nyx_trans_scale(NYX_VECTOR v, NYX_SCALE s) {
    return (NYX_TRANSFORM){.trans_scale={.type=NYX_TRANSFORM_TYPE_TRANS_SCALE, .trans=v, .scale=s}};
}

NYX_TRANSFORM nyx_trans_simple_camera(NYX_VECTOR v, int32_t focus) {
    return (NYX_TRANSFORM){.simple_camera={.type=NYX_TRANSFORM_TYPE_SIMPLE_CAMERA, .trans=v, .w=nyx_width(), .h=nyx_height(), .focus=focus}};
}
