#include "nyx/nyx.h"

NYX_VECTOR nyx_vector_opp(NYX_VECTOR v) {
    NYX_VECTOR res;
    int i;

    for(i=0; i<DIMENSIONS; ++i)
        res.a[i] = -v.a[i];
    return res;
}

void nyx_vector_opp_ptr(NYX_VECTOR *v) {
    int i;

    for(i=0; i<DIMENSIONS; ++i)
        v->a[i] = -v->a[i];
}

NYX_VECTOR nyx_vector_add(NYX_VECTOR v1, NYX_VECTOR v2) {
    NYX_VECTOR res;
    int i;

    for(i=0; i<DIMENSIONS; ++i)
        res.a[i] = v1.a[i] + v2.a[i];
    return res;
}

void nyx_vector_add_ptr(NYX_VECTOR *v1, NYX_VECTOR v2) {
    int i;

    for(i=0; i<DIMENSIONS; ++i)
        v1->a[i] += v2.a[i];
}

NYX_VECTOR nyx_vector_imul(NYX_VECTOR v, int32_t s) {
    NYX_VECTOR res;
    int i;

    for(i=0; i<DIMENSIONS; ++i)
        res.a[i] = v.a[i] * s;
    return res;
}

void nyx_vector_imul_ptr(NYX_VECTOR *v, int32_t s) {
    int i;

    for(i=0; i<DIMENSIONS; ++i)
        v->a[i] *= s;
}

NYX_VECTOR nyx_vector_mul(NYX_VECTOR v, float s) {
    NYX_VECTOR res;
    int i;

    for(i=0; i<DIMENSIONS; ++i)
        res.a[i] = floorf(v.a[i] * s);
    return res;
}

void nyx_vector_mul_ptr(NYX_VECTOR *v, float s) {
    int i;

    for(i=0; i<DIMENSIONS; ++i)
        v->a[i] = floorf(v->a[i] * s);
}

NYX_VECTOR nyx_vector_iscale(NYX_VECTOR v, NYX_ISCALE s) {
    NYX_VECTOR res;
    int i;

    for(i=0; i<DIMENSIONS; ++i)
        res.a[i] = v.a[i] * s.a[i];
    return res;
}

void nyx_vector_iscale_ptr(NYX_VECTOR *v, NYX_ISCALE s) {
    int i;

    for(i=0; i<DIMENSIONS; ++i)
        v->a[i] *= s.a[i];
}

NYX_VECTOR nyx_vector_scale(NYX_VECTOR v, NYX_SCALE s) {
    NYX_VECTOR res;
    int i;

    for(i=0; i<DIMENSIONS; ++i)
        res.a[i] = floorf(v.a[i] * s.a[i]);
    return res;
}

void nyx_vector_scale_ptr(NYX_VECTOR *v, NYX_SCALE s) {
    int i;

    for(i=0; i<DIMENSIONS; ++i)
        v->a[i] = floorf(v->a[i] * s.a[i]);
}

NYX_VECTOR nyx_linear(NYX_VECTOR v, NYX_LINEAR_MAP map) {
    NYX_VECTOR res;
    int i;
    int j;

    for(i=0; i<DIMENSIONS; ++i)
    {
        float f = 0;

        for(j=0; j<DIMENSIONS; ++j)
            f += v.a[j] * map.mat[i][j];
        res.a[i] = floorf(f);
    }
    return res;
}

NYX_VECTOR nyx_affine(NYX_VECTOR v, NYX_AFFINE_MAP map) {
    return nyx_linear(nyx_vector_add(v, map.trans), map.linear_map);
}

NYX_ISCALE nyx_iso_iscale(int32_t s) {
    NYX_ISCALE res;
    int i;

    for(i=0; i<DIMENSIONS; ++i)
        res.a[i] = s;
    return res;
}

NYX_SCALE nyx_iso_scale(float s) {
    NYX_SCALE res;
    int i;

    for(i=0; i<DIMENSIONS; ++i)
        res.a[i] = s;
    return res;
}
