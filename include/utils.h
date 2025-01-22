#ifndef UTILS_H
#define UTILS_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>

#include "camera.h"

void drawCircle(SDL_Renderer *renderer, float r, float x, float y);
void drawRectangle(SDL_Renderer *renderer, float x, float y, float w, float h);
SDL_Surface* CreateSurfaceFromMemory(const unsigned char* data, size_t size);
void drawGrid(SDL_Renderer *renderer, Camera * camera, int w, int h);
void drawBackground(SDL_Renderer *renderer, Camera * camera, int w, int h);
SDL_Color RGBA(int r, int g, int b, int a);
void drawText(void * data, const char * text, TTF_Font * font, float x, float y, SDL_Color color, bool centered);
#endif