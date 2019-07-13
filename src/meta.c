#include "nyx/nyx.h"

#include "_const.h"
#include "_event.h"
#include "_graphics.h"
#include "_keyboard.h"
#include "_random.h"
#include "_render.h"

static bool exitflag = false;

static SDL_TimerID timer;

static float fps = DEFAULT_FPS;
static uint64_t tic_time;
static uint64_t timer_tic;
static uint32_t timer_event_type;

static uint64_t tic;
static float max_desync = DEFAULT_MAX_DESYNC;
static uint64_t skipped_frames;

static void (*tic_handler)(void);
static int (*event_handler)(NYX_EVENT *);
static void (*renderer)(void);

static bool rendered;

static void timer_event(void) {
    SDL_Event event;

    SDL_memset(&event, 0, sizeof(event));
    event.type = timer_event_type;
    SDL_PushEvent(&event);
}

static int64_t tic_remaining(void) {
    int64_t remaining = (tic_time + (uint64_t)(SDL_GetPerformanceFrequency() / fps) - SDL_GetPerformanceCounter()) * 1000 / SDL_GetPerformanceFrequency();

    return remaining < 1 ? 1 : remaining;
}

static uint32_t timer_tic_handler(uint32_t interval, void *param) {
    interval = 0;
    param = 0;
    tic_time += (uint64_t)(SDL_GetPerformanceFrequency() / fps);
    ++timer_tic;
    timer_event();
    return tic_remaining();
}

static int timer_start(void) {
    tic_time = SDL_GetPerformanceCounter();
    timer = SDL_AddTimer(tic_remaining(), timer_tic_handler, 0);
    if(!timer)
        return -1;
    timer_tic = 0;
    timer_event_type = SDL_RegisterEvents(1);
    if(timer_event_type == 0xFFFFFFFF)
        return -1;
    timer_event();
    return 0;
}

void _renderer(void) {
    renderer();
}

int nyx_init(void) {
    if(_rnd_init())
        return -1;
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER))
        return -1;
    if(_init_graphics())
        return -1;
    if(_init_display())
        return -1;
    if(_init_palette())
        return -1;
    return 0;
}

int nyx_start(void) {
    if(timer_start())
        return -1;
    while(!exitflag)
    {
        SDL_Event event;

        if(!rendered && !SDL_PollEvent(0))
        {
            if(renderer)
                _render();
            rendered = true;
        }
        if(!SDL_WaitEvent(&event))
            return -1;
        if(event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
        {
            NYX_EVENT e = _translate_event(&event);
            
            if(e.type && event_handler)
                event_handler(&e);
        }
        else if(event.type == timer_event_type)
        {
            if(!rendered)
                ++skipped_frames;
            if(timer_tic > tic + max_desync * fps)
                return NYX_ERROR_DESYNC;
            _update_keyboard_state();
            nyx_select_layer(0);
            if(tic_handler)
                tic_handler();
            if(tic == timer_tic)
                rendered = false;
            else
                ++skipped_frames;
            ++tic;
        }
    }
    return 0;
}

void nyx_end(void) {
    exitflag = true;
}

uint64_t nyx_tic(void) {
    return tic;
}

void nyx_set_tic_handler(void (*f)(void)) {
    tic_handler = f;
}

void nyx_set_event_handler(int (*f)(NYX_EVENT *)) {
    event_handler = f;
}

void nyx_set_renderer(void (*f)(void)) {
    renderer = f;
}
