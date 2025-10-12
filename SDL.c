#include <SDL3/SDL_init.h>

#include "SDL.h"

int initialise_SDL_video (SDL_Window **p_window, SDL_Surface **p_window_surface, SDL_Renderer **p_renderer, const char *title, const int WIN_WIDTH, const int WIN_HEIGHT) {
/* Initialise an SDL window
 *
 * INPUT:
 *     p_window          - a pointer to where window should be assigned
 *     p_window_surface  - a pointer to where window_surface should be assigned
 *     p_renderer        - a pointer to where renderer should be assigned
 */

    if (!SDL_Init(SDL_INIT_VIDEO)) {return -1;}

    SDL_Window *window = SDL_CreateWindow(title, WIN_WIDTH, WIN_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_Surface *window_surface = SDL_GetWindowSurface(window);

    *p_window = window; *p_window_surface = window_surface; *p_renderer = renderer;

    return 0;
}
