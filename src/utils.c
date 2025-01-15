#include "../include/utils.h"

#include <SDL3_image/SDL_image.h>

#define CIRCLE_DRAW_SIDES 32
#define CIRCLE_DRAW_SIDES_LEN (CIRCLE_DRAW_SIDES + 1)

SDL_Surface* CreateSurfaceFromMemory(const unsigned char* data, size_t size) {
    SDL_IOStream* rw = SDL_IOFromMem((void*)data, size);  // Crée un flux mémoire à partir des données
    if (!rw) {
        return NULL;
    }
    SDL_Surface* surface = IMG_Load_IO(rw, 1);  // Charge l'image en surface
    return surface;
}

void drawCircle(SDL_Renderer *renderer, float r, float x, float y) {
    float ang;
    SDL_FPoint points[CIRCLE_DRAW_SIDES_LEN];
    int i;
    for (i = 0; i < CIRCLE_DRAW_SIDES_LEN; i++) {
        ang = 2.0f * SDL_PI_F * (float)i / (float)CIRCLE_DRAW_SIDES;
        points[i].x = x + r * SDL_cosf(ang);
        points[i].y = y + r * SDL_sinf(ang);
    }
    SDL_RenderLines(renderer, (const SDL_FPoint*)&points, CIRCLE_DRAW_SIDES_LEN);
}

void drawRectangle(SDL_Renderer *renderer, int x, int y, int w, int h) {
    SDL_FRect rect = { x, y, w, h };
    SDL_RenderFillRect(renderer, &rect);
}

void drawGrid(SDL_Renderer *renderer, Camera * camera, int w, int h) {
    SDL_SetRenderDrawColor(renderer, 44, 44, 44, 255);

    int startX = ((int)camera->x / 50) * 50;
    if (camera->x < 0) startX -= 50;
    int startY = ((int)camera->y / 50) * 50;
    if (camera->y < 0) startY -= 50;

    int endX = camera->x + w;
    int endY = camera->y + h;

    // Dessiner les lignes verticales
    for (int x = startX; x < endX; x += 50) {
        if (x >= camera->x && x <= camera->x + w) {
            int screenX = x - camera->x;
            SDL_RenderLine(renderer, screenX, 0, screenX, h);
        }
    }

    // Dessiner les lignes horizontales
    for (int y = startY; y < endY; y += 50) {
        if (y >= camera->y && y <= camera->y + h) {
            int screenY = y - camera->y;
            SDL_RenderLine(renderer, 0, screenY, w, screenY);
        }
    }
}


