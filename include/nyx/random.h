#ifndef NYX_RANDOM_H
#define NYX_RANDOM_H

uint8_t nyx_random_uint8(void);
uint32_t nyx_random_uint32(void);
double nyx_random_double(void);
int32_t nyx_random_range(int32_t from, int32_t to);
float nyx_random_float_range(float from, float to);
NYX_COLOR nyx_random_color(void);

#endif
