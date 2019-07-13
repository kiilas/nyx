#include "nyx/nyx.h"

#include "_pipeline.h"

static NYX_TRANSFORM view_transform;

NYX_TRANSFORM nyx_view_transform(void) {
    return view_transform;
}

void nyx_set_view_transform(NYX_TRANSFORM t) {
    view_transform = t;
    _update_pipeline();
}

int nyx_view_pan(int32_t dx, int32_t dy) {
    NYX_VECTOR v = nyx_vector(dx, dy);

    return nyx_view_panv(v);
}

int nyx_view_pan3d(int32_t dx, int32_t dy, int32_t dz) {
    NYX_VECTOR v = nyx_vector(dx, dy, dz);

    return nyx_view_panv(v);
}

int nyx_view_panv(NYX_VECTOR v) {
    NYX_VECTOR v_opp = nyx_vector_opp(v);

    switch(view_transform.type) {
        case NYX_TRANSFORM_TYPE_INBOUNDS:
        case NYX_TRANSFORM_TYPE_NONE:
            view_transform = nyx_trans(v_opp);
            _update_pipeline();
            return 0;
        case NYX_TRANSFORM_TYPE_TRANS:
            nyx_vector_add_ptr(&view_transform.trans.trans, v_opp);
            _update_pipeline();
            return 0;
        case NYX_TRANSFORM_TYPE_TRANS_ISCALE:
            nyx_vector_add_ptr(&view_transform.trans_iscale.trans, v_opp);
            _update_pipeline();
            return 0;
        case NYX_TRANSFORM_TYPE_TRANS_SCALE:
            nyx_vector_add_ptr(&view_transform.trans_scale.trans, v_opp);
            _update_pipeline();
            return 0;
        case NYX_TRANSFORM_TYPE_SIMPLE_CAMERA:
            nyx_vector_add_ptr(&view_transform.simple_camera.trans, v_opp);
            _update_pipeline();
            return 0;
        case NYX_TRANSFORM_TYPE_AFFINE:
            nyx_vector_add_ptr(&view_transform.affine.map.trans, v_opp);
            _update_pipeline();
            return 0;
    }
    return -1;
}
