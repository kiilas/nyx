#include "nyx/nyx.h"

#include "SDL2/SDL.h"

static uint8_t const *state;
static int state_len;

void _update_keyboard_state(void) {
    state = SDL_GetKeyboardState(&state_len);
}

bool nyx_key_pressed(unsigned code) {
    if(code < (unsigned)state_len)
        return state[code];
    return false;
}
