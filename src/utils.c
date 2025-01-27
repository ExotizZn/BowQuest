#include <SDL3_image/SDL_image.h>
#include <string.h>
#include <stdbool.h>

#include "../libs/SDL3_gfx/SDL3_gfxPrimitives.h"

#include "../include/appstate.h"
#include "../include/utils.h"

void fillRect(SDL_Renderer *renderer, float x, float y, float w, float h) {
    SDL_FRect rect = { x, y, w, h };
    SDL_RenderFillRect(renderer, &rect);
}

void roundRect(SDL_Renderer *renderer, float x, float y, float w, float h, float radius, SDL_Color color) {
    roundedBoxRGBA(renderer, x, y, x+w, y+h, radius, color.r, color.g, color.b, color.a);
}

void drawFilledRotatedRect(SDL_Renderer *renderer, float x, float y, float w, float h, float angle, SDL_Color color) {
    // Coordonnées des coins du rectangle par rapport à son centre
    float half_w = w / 2.0f;
    float half_h = h / 2.0f;

    float radians = angle * (M_PI / 180.0f);
    float cos_theta = cosf(radians);
    float sin_theta = sinf(radians);

    // Calculer les coins après rotation
    float x1 = x + (-half_w * cos_theta - -half_h * sin_theta); // Coin supérieur gauche
    float y1 = y + (-half_w * sin_theta + -half_h * cos_theta);

    float x2 = x + (half_w * cos_theta - -half_h * sin_theta); // Coin supérieur droit
    float y2 = y + (half_w * sin_theta + -half_h * cos_theta);

    float x3 = x + (half_w * cos_theta - half_h * sin_theta); // Coin inférieur droit
    float y3 = y + (half_w * sin_theta + half_h * cos_theta);

    float x4 = x + (-half_w * cos_theta - half_h * sin_theta); // Coin inférieur gauche
    float y4 = y + (-half_w * sin_theta + half_h * cos_theta);

    // Points du polygone
    Sint16 vx[4] = {(Sint16)x1, (Sint16)x2, (Sint16)x3, (Sint16)x4};
    Sint16 vy[4] = {(Sint16)y1, (Sint16)y2, (Sint16)y3, (Sint16)y4};

    // Dessiner le polygone rempli
    filledPolygonRGBA(renderer, vx, vy, 4, color.r, color.g, color.b, color.a);
}

void drawLine(SDL_Renderer *renderer, float x1, float y1, float x2, float y2, SDL_Color color) {
    aalineRGBA(renderer, x1, y1, x2, y2, color.r, color.g, color.b, color.a);
}

void circle(SDL_Renderer *renderer, float x, float y, float radius, SDL_Color color) {
    aacircleRGBA(renderer, x, y, radius, color.r, color.g, color.b, color.a);
}

void filledCircle(SDL_Renderer *renderer, float x, float y, float radius, SDL_Color color) {
    filledCircleRGBA(renderer, x, y, radius, color.r, color.g, color.b, color.a);
}

SDL_Surface* CreateSurfaceFromMemory(const unsigned char* data, size_t size) {
    SDL_IOStream* rw = SDL_IOFromMem((void*)data, size);  // Crée un flux mémoire à partir des données
    if (!rw) return NULL;
    
    SDL_Surface* surface = IMG_Load_IO(rw, 1);  // Charge l'image en surface
    return surface;
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

// Fonction pour calculer un point sur une courbe de Bézier quadratique
Point bezier_quadratic(Point p0, Point p1, Point p2, float t) {
    Point result;
    result.x = (1 - t) * (1 - t) * p0.x + 2 * (1 - t) * t * p1.x + t * t * p2.x;
    result.y = (1 - t) * (1 - t) * p0.y + 2 * (1 - t) * t * p1.y + t * t * p2.y;
    return result;
}