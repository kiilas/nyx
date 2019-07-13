#ifndef NYX_TRANSFORM_H
#define NYX_TRANSFORM_H

#include "nyx/linear.h"
#include "nyx/symmetry.h"

#define NYX_TRANSFORM_TYPE_INBOUNDS      0
#define NYX_TRANSFORM_TYPE_NONE          1
#define NYX_TRANSFORM_TYPE_TRANS         2
#define NYX_TRANSFORM_TYPE_TRANS_ISCALE  3
#define NYX_TRANSFORM_TYPE_TRANS_SCALE   4
#define NYX_TRANSFORM_TYPE_AFFINE        5
#define NYX_TRANSFORM_TYPE_SIMPLE_CAMERA 6
#define NYX_TRANSFORM_TYPE_FUNC          7

typedef uint8_t NYX_TRANSFORM_TYPE;

typedef struct NYX_TRANSFORM_INBOUNDS {
    NYX_TRANSFORM_TYPE type;
} NYX_TRANSFORM_INBOUNDS;

typedef struct NYX_TRANSFORM_NONE {
    NYX_TRANSFORM_TYPE type;
} NYX_TRANSFORM_NONE;

typedef struct NYX_TRANSFORM_TRANS {
    NYX_TRANSFORM_TYPE type;
    NYX_VECTOR         trans;
} NYX_TRANSFORM_TRANS;

typedef struct NYX_TRANSFORM_TRANS_ISCALE {
    NYX_TRANSFORM_TYPE type;
    NYX_VECTOR         trans;
    NYX_ISCALE         scale;
} NYX_TRANSFORM_TRANS_ISCALE;

typedef struct NYX_TRANSFORM_TRANS_SCALE {
    NYX_TRANSFORM_TYPE type;
    NYX_VECTOR         trans;
    NYX_SCALE          scale;
} NYX_TRANSFORM_TRANS_SCALE;

typedef struct NYX_TRANSFORM_AFFINE {
    NYX_TRANSFORM_TYPE type;
    NYX_AFFINE_MAP     map;
} NYX_TRANSFORM_AFFINE;

typedef struct NYX_TRANSFORM_SIMPLE_CAMERA {
    NYX_TRANSFORM_TYPE type;
    NYX_VECTOR         trans;
    int32_t            w;
    int32_t            h;
    int32_t            focus;
} NYX_TRANSFORM_SIMPLE_CAMERA;

typedef struct NYX_TRANSFORM_FUNC {
    NYX_TRANSFORM_TYPE type;
    NYX_VECTOR (*f)(NYX_VECTOR);
} NYX_TRANSFORM_FUNC;

typedef union NYX_TRANSFORM {
    NYX_TRANSFORM_TYPE          type;
    NYX_TRANSFORM_INBOUNDS      inbounds;
    NYX_TRANSFORM_NONE          none;
    NYX_TRANSFORM_TRANS         trans;
    NYX_TRANSFORM_TRANS_ISCALE  trans_iscale;
    NYX_TRANSFORM_TRANS_SCALE   trans_scale;
    NYX_TRANSFORM_AFFINE        affine;
    NYX_TRANSFORM_SIMPLE_CAMERA simple_camera;
    NYX_TRANSFORM_FUNC          func;
} NYX_TRANSFORM;

#define NYX_TRANSFORM_IDENTITY ((NYX_TRANSFORM){.inbounds={.type=NYX_TRANSFORM_TYPE_INBOUNDS}})

NYX_VECTOR nyx_transform(NYX_VECTOR v, const NYX_TRANSFORM *t);
NYX_VECTOR nyx_transform_scale(NYX_VECTOR v, const NYX_TRANSFORM *t);

NYX_TRANSFORM nyx_trans(NYX_VECTOR v);
NYX_TRANSFORM nyx_trans_iscale(NYX_VECTOR v, NYX_ISCALE s);
NYX_TRANSFORM nyx_trans_scale(NYX_VECTOR v, NYX_SCALE s);
NYX_TRANSFORM nyx_trans_simple_camera(NYX_VECTOR v, int32_t focus);

#endif
