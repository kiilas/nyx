#ifndef NYX_SYMMETRY_H
#define NYX_SYMMETRY_H

#include <stdint.h>

#define NYX_SYMMETRY_ID         0x00
#define NYX_SYMMETRY_FLIP_H     0x01
#define NYX_SYMMETRY_FLIP_V     0x02
#define NYX_SYMMETRY_ROTATE_180 0x03
#define NYX_SYMMETRY_ROTATE_CW  0x04
#define NYX_SYMMETRY_ROTATE_CCW 0x07

typedef uint8_t NYX_SYMMETRY_D4;

#endif
