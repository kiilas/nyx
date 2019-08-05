#include "nyx/nyx.h"

#include "SDL2/SDL_image.h"

struct NYX_BITMAP {
    int       w;
    int       h;
    uint32_t *pixels;
};

static NYX_BITMAP *from_locked_surface(SDL_Surface *surface) {
    NYX_BITMAP *bitmap;
    int y;

    bitmap = nyx_create_bitmap(surface->w, surface->h);
    if(!bitmap)
        return 0;
    for(y=0; y<surface->h; ++y)
        memcpy(bitmap->pixels + y*bitmap->w,
               (uint8_t *)surface->pixels + y*surface->pitch,
               4 * bitmap->w);
    return bitmap;
}

static NYX_BITMAP *from_converted_surface(SDL_Surface *surface) {
    NYX_BITMAP *bitmap;

    if(SDL_LockSurface(surface))
        return 0;
    bitmap = from_locked_surface(surface);
    SDL_UnlockSurface(surface);
    return bitmap;
}

static NYX_BITMAP *from_surface(SDL_Surface *surface) {
    NYX_BITMAP  *bitmap;
    SDL_Surface *converted;

    converted = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
    if(!converted)
        return 0;
    bitmap = from_converted_surface(converted);
    SDL_FreeSurface(converted);
    return bitmap;
}

NYX_BITMAP *nyx_create_bitmap(int w, int h) {
    NYX_BITMAP *bitmap;

    bitmap = malloc(sizeof(NYX_BITMAP));
    if(!bitmap)
        return 0;
    bitmap->w = w;
    bitmap->h = h;
    bitmap->pixels = malloc(4 * w * h);
    if(!bitmap->pixels)
    {
        free(bitmap);
        return 0;
    }
    return bitmap;
}

void nyx_destroy_bitmap(NYX_BITMAP *bitmap) {
    if(!bitmap)
        return;
    free(bitmap->pixels);
    free(bitmap);
}

NYX_BITMAP *nyx_load_bitmap(const char *path) {
    NYX_BITMAP  *bitmap;
    SDL_Surface *surface;
    SDL_RWops   *rw;

    rw = SDL_RWFromFile(path, "rb");
    if(!rw)
        return 0;
    surface = IMG_LoadPNG_RW(rw);
    SDL_RWclose(rw);
    if(!surface)
        return 0;
    bitmap = from_surface(surface);
    SDL_FreeSurface(surface);
    return bitmap;
}

int nyx_get_bitmap_width(const NYX_BITMAP *bitmap) {
    if(!bitmap)
        return -1;
    return bitmap->w;
}

int nyx_get_bitmap_height(const NYX_BITMAP *bitmap) {
    if(!bitmap)
        return -1;
    return bitmap->h;
}

uint32_t nyx_get_bitmap_pixel_unsafe(const NYX_BITMAP *bitmap, int x, int y) {
    return bitmap->pixels[x + y*bitmap->w];
}

int nyx_get_bitmap_pixel(const NYX_BITMAP *bitmap, int x, int y, uint32_t *color) {
    if(!bitmap)
        return -1;
    if(x<0 || y<0 || x>=bitmap->w || y>=bitmap->h)
        return -1;
    *color = nyx_get_bitmap_pixel_unsafe(bitmap, x, y);
    return 0;
}

uint8_t nyx_get_bitmap_pixel_alpha_unsafe(const NYX_BITMAP *bitmap, int x, int y) {
    return bitmap->pixels[x + y*bitmap->w] >> 24;
}
