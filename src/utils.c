#include "../include/appstate.h"
#include "../include/utils.h"

#include <SDL3_image/SDL_image.h>
#include <string.h>
#include <stdbool.h>

#define CIRCLE_DRAW_SIDES 32
#define CIRCLE_DRAW_SIDES_LEN (CIRCLE_DRAW_SIDES + 1)

SDL_Surface* CreateSurfaceFromMemory(const unsigned char* data, size_t size) {
    SDL_IOStream* rw = SDL_IOFromMem((void*)data, size);  // Crée un flux mémoire à partir des données
    if (!rw) return NULL;
    
    SDL_Surface* surface = IMG_Load_IO(rw, 1);  // Charge l'image en surface
    return surface;
}

void drawCircle(SDL_Renderer *renderer, float r, float x, float y) {
    SDL_FPoint points[CIRCLE_DRAW_SIDES_LEN];

    for (int i = 0; i < CIRCLE_DRAW_SIDES_LEN; i++) {
        float ang = 2.0f * SDL_PI_F * i / CIRCLE_DRAW_SIDES;
        points[i].x = x + r * SDL_cosf(ang);
        points[i].y = y + r * SDL_sinf(ang);
    }

    SDL_RenderLines(renderer, points, CIRCLE_DRAW_SIDES_LEN);
}

void drawRectangle(SDL_Renderer *renderer, float x, float y, float w, float h) {
    SDL_FRect rect = { x, y, w, h };
    SDL_RenderFillRect(renderer, &rect);
}

void drawGrid(SDL_Renderer *renderer, Camera * camera, int w, int h) {
    SDL_SetRenderDrawColor(renderer, 44, 44, 44, 255);

    int startX = (int)camera->x / 50 * 50;
    int startY = (int)camera->y / 50 * 50;

    int endX = camera->x + w;
    int endY = camera->y + h;

    for (int x = startX; x <= endX; x += 50) {
        int screenX = x - camera->x;
        SDL_RenderLine(renderer, screenX, 0, screenX, h);
    }

    for (int y = startY; y <= endY; y += 50) {
        int screenY = y - camera->y;
        SDL_RenderLine(renderer, 0, screenY, w, screenY);
    }
}

void drawBackground(SDL_Renderer *renderer, Camera *camera, int w, int h) {
    int startX = ((int)camera->x / 50) * 50;
    if (camera->x < 0) startX -= 50;
    int startY = ((int)camera->y / 50) * 50;
    if (camera->y < 0) startY -= 50;

    int endX = camera->x + w;
    int endY = camera->y + h;

    // Parcourir la grille et dessiner des rectangles
    for (int x = startX; x <= endX; x += 50) {
        for (int y = startY; y <= endY; y += 50) {
            if (((x / 50) + (y / 50)) % 2 == 0) {
                SDL_SetRenderDrawColor(renderer, 126, 200, 0, 255);
            } else {
                SDL_SetRenderDrawColor(renderer, 132, 205, 0, 255);
            }
            SDL_FRect rect = { x - camera->x, y - camera->y, 50, 50 };
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

SDL_Color RGBA(int r, int g, int b, int a) {
    return (SDL_Color){r, g, b, a};
}

void drawText(void * data, const char * text, TTF_Font * font, float x, float y, SDL_Color color, bool centered) {
    if(!text || *text == '\0') return;
    
    AppState *as = (AppState *)data;

    SDL_Surface *textSurface = TTF_RenderText_Blended(font, text, 0, color);
    if (!textSurface) return;

    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(as->renderer, textSurface);
    SDL_DestroySurface(textSurface);
    if (!textTexture) return;

    SDL_FRect textRect = {
        .x = centered ? x - textTexture->w / 2.0f : x,
        .y = centered ? y - textTexture->h / 2.0f : y,
        .w = textTexture->w,
        .h = textTexture->h
    };

    SDL_RenderTexture(as->renderer, textTexture, NULL, &textRect);
    SDL_DestroyTexture(textTexture);
}