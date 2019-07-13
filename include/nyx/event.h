#ifndef NYX_EVENT_H
#define NYX_EVENT_H

#define NYX_EVENT_KEY_DOWN 0x10
#define NYX_EVENT_KEY_UP   0x11
#define NYX_EVENT_KEY_CHAR 0x12

typedef unsigned NYX_EVENT_TYPE;

typedef struct NYX_KEYBOARD_EVENT {
    NYX_EVENT_TYPE type;
    int code;
    uint32_t unichar;
} NYX_KEYBOARD_EVENT;

typedef union NYX_EVENT {
    NYX_EVENT_TYPE type;
    NYX_KEYBOARD_EVENT keyboard;
} NYX_EVENT;

#endif
