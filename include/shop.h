#ifndef SHOP_H
#define SHOP_H

#include <SDL3/SDL.h>
#include "../include/appstate.h"

extern const unsigned int Shop_png_len;
extern const unsigned char Shop_png[];

void drawShop(SDL_Renderer *renderer, AppState *as);

#endif