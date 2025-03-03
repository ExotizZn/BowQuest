#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include <SDL3/SDL.h>
#include "../include/appstate.h"

extern const unsigned int Equipment_png_len;
extern const unsigned char Equipment_png[];

void drawEquipment(SDL_Renderer *renderer, AppState *as);

#endif