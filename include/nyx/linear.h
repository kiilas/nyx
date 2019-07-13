#ifndef NYX_LINEAR_H
#define NYX_LINEAR_H

#include <stdint.h>

#define DIMENSIONS 3

#define NYX_VECTOR_ZERO ((NYX_VECTOR){0})

typedef struct NYX_VECTOR {
    int32_t a[DIMENSIONS];
} NYX_VECTOR;

typedef struct NYX_ISCALE {
    int32_t a[DIMENSIONS];
} NYX_ISCALE;

typedef struct NYX_SCALE {
    float a[DIMENSIONS];
} NYX_SCALE;

typedef struct NYX_LINEAR_MAP {
    float mat[DIMENSIONS][DIMENSIONS];
} NYX_LINEAR_MAP;

typedef struct NYX_AFFINE_MAP {
    NYX_VECTOR     trans;
    NYX_LINEAR_MAP linear_map;
} NYX_AFFINE_MAP;

#define nyx_vector(...) ((NYX_VECTOR){__VA_ARGS__})
#define nyx_iscale(...) ((NYX_ISCALE){__VA_ARGS__})
#define nyx_scale(...) ((NYX_SCALE){__VA_ARGS__})

NYX_VECTOR nyx_vector_opp(NYX_VECTOR v);
void nyx_vector_opp_ptr(NYX_VECTOR *v);
NYX_VECTOR nyx_vector_add(NYX_VECTOR v1, NYX_VECTOR v2);
void nyx_vector_add_ptr(NYX_VECTOR *v1, NYX_VECTOR v2);
NYX_VECTOR nyx_vector_imul(NYX_VECTOR v, int32_t s);
void nyx_vector_imul_ptr(NYX_VECTOR *v, int32_t s);
NYX_VECTOR nyx_vector_mul(NYX_VECTOR v, float s);
void nyx_vector_mul_ptr(NYX_VECTOR *v, float s);
NYX_VECTOR nyx_vector_iscale(NYX_VECTOR v, NYX_ISCALE s);
void nyx_vector_iscale_ptr(NYX_VECTOR *v, NYX_ISCALE s);
NYX_VECTOR nyx_vector_scale(NYX_VECTOR v, NYX_SCALE s);
void nyx_vector_scale_ptr(NYX_VECTOR *v, NYX_SCALE s);
NYX_VECTOR nyx_linear(NYX_VECTOR v, NYX_LINEAR_MAP map);
NYX_VECTOR nyx_affine(NYX_VECTOR v, NYX_AFFINE_MAP map);

NYX_ISCALE nyx_iso_iscale(int32_t s);

NYX_SCALE  nyx_iso_scale(float s);

#endif
