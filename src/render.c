#include "nyx/nyx.h"

#include "_graphics.h"
#include "_meta.h"
#include "_render.h"

void _render(void) {
    _lock_screen();
    _renderer();
    _unlock_screen();
    _flip();
}
