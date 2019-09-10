#ifndef NYX__CONST_H
#define NYX__CONST_H

#define DEFAULT_SCREEN_WIDTH 320
#define DEFAULT_SCREEN_HEIGHT 240
#define DEFAULT_FULLSCREEN false

#define DEFAULT_FPS 60.0
#define DEFAULT_MAX_DESYNC 15.0

#define DEFAULT_BACKGROUND_COLOR {0, 0.0}
#define DEFAULT_NUM_RAMPS 9
#define DEFAULT_NUM_VALUES 5

const static uint8_t DEFAULT_RAMPS[DEFAULT_NUM_RAMPS][DEFAULT_NUM_VALUES][3] = {
    {{  0,   0,   0}, { 32,  32,  32}, { 64,  64,  64}, {128, 128, 128}, {255, 255, 255}},
    {{  0,   0,   0}, { 24,   0,  32}, { 48,   0,  64}, { 96,   0, 128}, {192,   0, 255}},
    {{  0,   0,   0}, { 32,  16,  24}, { 64,  32,  48}, {128,  64,  96}, {255, 128, 192}},
    {{  0,   0,   0}, { 32,   8,   0}, { 64,  16,   0}, {128,  32,   0}, {255,  64,   0}},
    {{  0,   0,   0}, { 32,  32,  16}, { 64,  64,  32}, {128, 128,  64}, {255, 255, 128}},
    {{  0,   0,   0}, {  8,  32,   0}, { 16,  64,   0}, { 32, 128,   0}, { 64, 255,   0}},
    {{  0,   0,   0}, { 16,  32,  24}, { 32,  64,  48}, { 64, 128,  96}, {128, 255, 192}},
    {{  0,   0,   0}, {  0,  24,  32}, {  0,  48,  64}, {  0,  96, 128}, {  0, 192, 255}},
    {{  0,   0,   0}, { 16,  16,  32}, { 32,  32,  64}, { 64,  64, 128}, {128, 128, 255}}
};

#define DEFAULT_SQUISH 1.0

#define DEFAULT_FONT_H_SPACING 0.0625
#define DEFAULT_FONT_V_SPACING 0.0625

#endif
