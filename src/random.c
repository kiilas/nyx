#include "nyx/nyx.h"

#include <stdbool.h>

static FILE *random_file;
static bool initialised = false;

int _rnd_init(void) {
    if(initialised)
        return 0;
    random_file = fopen("/dev/urandom", "rb");
    if(!random_file)
        return -1;
    initialised = true;
    return 0;
}

uint8_t nyx_random_uint8(void) {
    return fgetc(random_file);
}

uint32_t nyx_random_uint32(void) {
    uint32_t res;

    fread((void *)&res, 4, 1, random_file);
    return res;
}

double nyx_random_double(void) {
    return (double)nyx_random_uint32() / 4294967296;
}

int32_t nyx_random_range(int32_t from, int32_t to) {
    return from + nyx_random_uint32() % (to - from + 1);
}

float nyx_random_float_range(float from, float to) {
    return from + nyx_random_double() * (to - from);
}

NYX_COLOR nyx_random_color(void) {
    int ramp = nyx_random_range(0, nyx_num_ramps() - 1);
    uint8_t value = nyx_random_uint8();

    return (NYX_COLOR){ramp, value};
}
