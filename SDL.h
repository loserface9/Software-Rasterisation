#ifndef SDL_H
#define SDL_H

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

int initialise_SDL_video (
    SDL_Window **p_window, SDL_Surface **p_window_surface, SDL_Renderer **p_renderer,
    const char *title, const int WIN_WIDTH, const int WIN_HEIGHT
);

#endif
