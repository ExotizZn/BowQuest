#include <SDL3/SDL.h>
#include "../include/camera.h"

#define CHECK_SDL(x, msg) do { if (!(x)) { SDL_Log(msg ": %s", SDL_GetError()); return SDL_APP_FAILURE; } } while (0)

void initCamera(Camera **camera) {
    *camera = SDL_calloc(1, sizeof(Camera));
}