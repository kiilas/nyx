#include "nyx/nyx.h"

#include "_const.h"

#include "SDL2/SDL.h"

static bool initialised = false;

static int screen_width = DEFAULT_SCREEN_WIDTH;
static int screen_height = DEFAULT_SCREEN_HEIGHT;

static SDL_Window *window;
static SDL_Renderer *window_renderer;

static SDL_Texture *screen_texture;

static SDL_Surface *screen;

static uint8_t num_ramps;
static uint8_t num_values;
static uint32_t *ramps;

static float dither_squish = DEFAULT_SQUISH;

static NYX_COLOR background_color;

static const bool dither_table[17][16] = {
    {
        false, false, false, false,
        false, false, false, false,
        false, false, false, false,
        false, false, false, false
    },
    {
         true, false, false, false,
        false, false, false, false,
        false, false, false, false,
        false, false, false, false
    },
    {
         true, false, false, false,
        false, false, false, false,
        false, false,  true, false,
        false, false, false, false
    },
    {
         true, false,  true, false,
        false, false, false, false,
        false, false,  true, false,
        false, false, false, false
    },
    {
         true, false,  true, false,
        false, false, false, false,
         true, false,  true, false,
        false, false, false, false
    },
    {
         true, false,  true, false,
        false,  true, false, false,
         true, false,  true, false,
        false, false, false, false
    },
    {
         true, false,  true, false,
        false,  true, false, false,
         true, false,  true, false,
        false, false, false,  true
    },
    {
         true, false,  true, false,
        false,  true, false,  true,
         true, false,  true, false,
        false, false, false,  true
    },
    {
         true, false,  true, false,
        false,  true, false,  true,
         true, false,  true, false,
        false,  true, false,  true
    },
    {
         true,  true,  true, false,
        false,  true, false,  true,
         true, false,  true, false,
        false,  true, false,  true
    },
    {
         true,  true,  true, false,
        false,  true, false,  true,
         true, false,  true,  true,
        false,  true, false,  true
    },
    {
         true,  true,  true,  true,
        false,  true, false,  true,
         true, false,  true,  true,
        false,  true, false,  true
    },
    {
         true,  true,  true,  true,
        false,  true, false,  true,
         true,  true,  true,  true,
        false,  true, false,  true
    },
    {
         true,  true,  true,  true,
        false,  true,  true,  true,
         true,  true,  true,  true,
        false,  true, false,  true
    },
    {
         true,  true,  true,  true,
        false,  true,  true,  true,
         true,  true,  true,  true,
         true,  true, false,  true
    },
    {
         true,  true,  true,  true,
         true,  true,  true,  true,
         true,  true,  true,  true,
         true,  true, false,  true
    },
    {
         true,  true,  true,  true,
         true,  true,  true,  true,
         true,  true,  true,  true,
         true,  true,  true,  true
    },
};

int _init_graphics(void) {
    screen = SDL_CreateRGBSurfaceWithFormat(0, screen_width, screen_height, 8, SDL_PIXELFORMAT_RGBA32);
    if(!screen)
        return -1;
    SDL_FillRect(screen, NULL, 0);
    initialised = true;
    return 0;
}

int _init_display(void) {
    window = SDL_CreateWindow("nyx", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_width, screen_height, SDL_WINDOW_RESIZABLE);
    if(!window)
        return -1;

    window_renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(!window_renderer)
        return -1;

    screen_texture = SDL_CreateTexture(window_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, screen_width, screen_height);
    if(!screen_texture)
        return -1;

    SDL_SetRenderDrawColor(window_renderer, 0, 0, 0, 0);
    SDL_RenderClear(window_renderer);
    SDL_RenderPresent(window_renderer);

    return 0;
}

int _init_palette(void) {
    void *new_ramps;
    int ramp, val;

    num_ramps = DEFAULT_NUM_RAMPS;
    num_values = DEFAULT_NUM_VALUES;
    new_ramps = realloc(ramps, num_ramps * num_values * 4);
    if(!new_ramps)
        return -1;
    ramps = new_ramps;
    for(ramp=0; ramp<num_ramps; ++ramp)
        for(val=0; val<num_values; ++val)
        {
            const uint8_t *rgb = DEFAULT_RAMPS[ramp][val];

            ramps[val + ramp*num_values] = SDL_MapRGB(screen->format, rgb[0], rgb[1], rgb[2]);
        }
    return 0;
}

int _lock_screen(void) {
    return SDL_LockSurface(screen);
}

void _unlock_screen(void) {
    SDL_UnlockSurface(screen);
}

int _flip(void) {
    int display_width;
    int display_height;
    int scale;
    void *pixels;
    int pitch;
    int y;
    SDL_Rect r; // coords of rendered screen

    if(SDL_GetRendererOutputSize(window_renderer, &display_width, &display_height))
        return -1;

    // determine scale
    scale = display_width / screen_width;
    if(display_height / screen_height < scale)
        scale = display_height / screen_height;
    if(!scale)
        scale = 1;

    // determine top left corner of rendered screen
    r.x = (display_width - screen_width * scale) / 2;
    r.y = (display_height - screen_height * scale) / 2;
    r.w = screen_width * scale;
    r.h = screen_height * scale;

    if(SDL_LockTexture(screen_texture, NULL, &pixels, &pitch))
        return -1;
    for(y=0; y<screen_height; ++y)
        memcpy((uint8_t *)pixels+y*pitch, (uint8_t *)screen->pixels+y*screen->pitch, screen_width*4);
    SDL_UnlockTexture(screen_texture);

    SDL_SetRenderDrawColor(window_renderer, 0, 0, 0, 0);
    SDL_RenderClear(window_renderer);
    SDL_RenderCopy(window_renderer, screen_texture, 0, &r);
    SDL_RenderPresent(window_renderer);

    return 0;
}

uint32_t *_pixel(int x, int y) {
    return (uint32_t *)((uint8_t *)screen->pixels + x*4 + y*screen->pitch);
}

uint32_t _color(NYX_COLOR color, uint8_t x, uint8_t y) {
    uint8_t val;

    if(!dither_squish)
        val = num_values * color.value / 256;
    else
    {
        float v = (num_values-1) * color.value / 255.0f;
        float frac = v - floorf(v);

        val = floorf(v);
        if(frac >= 0.5 + 0.5/dither_squish)
            ++val;
        else if(frac >= 0.5 - 0.5/dither_squish)
        {
            int lvl = 16.0f * dither_squish * (frac - (0.5 - 0.5/dither_squish));

            if(dither_table[lvl][x%4 + 4*(y%4)])
                ++val;
        }
    }

    return ramps[val + color.ramp * num_values];
}

void _set_pixel_unsafe(int x, int y, NYX_COLOR color) {
    *_pixel(x, y) = _color(color, x, y);
}

void _set_pixel(int x, int y, NYX_COLOR color) {
    if(x>=0 && x<nyx_width() && y>=0 && y<nyx_height())
        _set_pixel_unsafe(x, y, color);
}

void _set_rect_unsafe(int x, int y, int w, int h, NYX_COLOR color) {
    int px, py;

    for(py=y; py<y+h; ++py)
        for(px=x; px<x+w; ++px)
            _set_pixel_unsafe(px, py, color);
}

void _set_rect(int x, int y, int w, int h, NYX_COLOR color) {
    if(x < 0)
    {
        w += x;
        x = 0;
    }
    if(x + w >= nyx_width())
        w = nyx_width() - x;
    if(w <= 0)
        return;
    if(y < 0)
    {
        h += y;
        y = 0;
    }
    if(y + h >= nyx_height())
        h = nyx_height() - y;
    if(h <= 0)
        return;
    _set_rect_unsafe(x, y, w, h, color);
}

int nyx_width(void) {
    return screen_width;
}

int nyx_height(void) {
    return screen_height;
}

int nyx_set_resolution(int width, int height) {
    if(initialised)
        return -1;
    screen_width = width;
    screen_height = height;
    return 0;
}

int nyx_num_ramps(void) {
    return num_ramps;
}

int nyx_num_values(void) {
    return num_values;
}

float nyx_dither_squish(void) {
    return dither_squish;
}

void nyx_set_dither_squish(float squish) {
    if(squish <= 0.0)
        squish = 0.0;
    else if(squish < 1.0)
        squish = 1.0;
    dither_squish = squish;
}

NYX_COLOR nyx_get_background_color(void) {
    return background_color;
}

void nyx_set_background_color(NYX_COLOR color) {
    background_color = color;
}

void nyx_screen_overlap(int x1, int y1, int x2, int y2, int *nx1, int *ny1, int *nx2, int *ny2) {
    nyx_overlap2d(x1, y1, x2, y2, 0, 0, nyx_width(), nyx_height(), nx1, ny1, nx2, ny2);
}
