#ifndef UTILS_H
#define UTILS_H

#include <SDL3/SDL.h>

#include "camera.h"

void drawCircle(SDL_Renderer *renderer, float r, float x, float y);
void drawRectangle(SDL_Renderer *renderer, float x, float y, float w, float h);
SDL_Surface* CreateSurfaceFromMemory(const unsigned char* data, size_t size);
void drawGrid(SDL_Renderer *renderer, Camera * camera, int w, int h);
void drawBackground(SDL_Renderer *renderer, Camera * camera, int w, int h);
void drawText(void * data, const char * text);
#endif