#ifndef MENU_H
#define MENU_H

#include <SDL3/SDL.h>

extern const unsigned int Menu_png_len;
extern const unsigned char Menu_png[];
void drawMenu(SDL_Renderer *renderer, AppState *as);

#endif