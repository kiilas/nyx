#ifndef NYX_MASK_H
#define NYX_MASK_H

typedef struct NYX_MASK NYX_MASK;

NYX_MASK *nyx_mask_init(int w, int h);
void nyx_mask_destroy(NYX_MASK *mask);

void *nyx_mask_bits_copy(const NYX_MASK *mask);

#endif
