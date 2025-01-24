#ifndef UTILS_H
#define UTILS_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>

#include "camera.h"

void roundRect(SDL_Renderer *renderer, float x, float y, float w, float h, float radius, SDL_Color color);
void fillRect(SDL_Renderer *renderer, float x, float y, float w, float h);
void drawLine(SDL_Renderer *renderer, float x1, float y1, float x2, float y2, SDL_Color color);
void circle(SDL_Renderer *renderer, float x, float y, float radius, SDL_Color color);
void filledCircle(SDL_Renderer *renderer, float x, float y, float radius, SDL_Color color);
SDL_Surface* CreateSurfaceFromMemory(const unsigned char* data, size_t size);
void drawGrid(SDL_Renderer *renderer, Camera * camera, int w, int h);
void drawBackground(SDL_Renderer *renderer, Camera * camera, int w, int h);
SDL_Color RGBA(int r, int g, int b, int a);
void drawText(void * data, const char * text, TTF_Font * font, float x, float y, SDL_Color color, bool centered);

#endif