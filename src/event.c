#include "nyx/nyx.h"

#include "_event.h"

NYX_EVENT _translate_event(SDL_Event *e) {
    NYX_EVENT nyx_event;

    switch(e->type) {
        case SDL_KEYDOWN:
            nyx_event.type = NYX_EVENT_KEY_DOWN;
            nyx_event.keyboard.code = e->key.keysym.scancode;
            break;
        case SDL_KEYUP:
            nyx_event.type = NYX_EVENT_KEY_UP;
            nyx_event.keyboard.code = e->key.keysym.scancode;
            break;
        default:
            nyx_event.type = 0;
    }
    return nyx_event;
}
