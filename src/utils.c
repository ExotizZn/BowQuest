#include <SDL3_image/SDL_image.h>
#include <string.h>
#include <stdbool.h>

#include "../libs/SDL3_gfx/SDL3_gfxPrimitives.h"

#include "../include/appstate.h"
#include "../include/utils.h"

#define DEG_TO_RAD (M_PI / 180.0f)

void fillRect(SDL_Renderer *renderer, float x, float y, float w, float h) {
    if (!renderer) return;
    SDL_FRect rect = { x, y, w, h };
    SDL_RenderFillRect(renderer, &rect);
}

void roundRect(SDL_Renderer *renderer, float x, float y, float w, float h, float radius, SDL_Color color) {
    if (!renderer) return;
    roundedBoxRGBA(renderer, x, y, x + w, y + h, radius, color.r, color.g, color.b, color.a);
}

void drawFilledRotatedRect(SDL_Renderer *renderer, float x, float y, float w, float h, float angle, SDL_Color color) {
    if (!renderer) return;

    float half_w = w * 0.5f, half_h = h * 0.5f;
    float radians = angle * DEG_TO_RAD;
    float cos_theta = cosf(radians), sin_theta = sinf(radians);

    // Precompute common terms
    float wx = half_w * cos_theta, wy = half_w * sin_theta;
    float hx = half_h * sin_theta, hy = half_h * cos_theta;

    Sint16 vx[4] = {
        (Sint16)(x - wx + hx),  // Top-left
        (Sint16)(x + wx + hx),  // Top-right
        (Sint16)(x + wx - hy),  // Bottom-right
        (Sint16)(x - wx - hy)   // Bottom-left
    };
    Sint16 vy[4] = {
        (Sint16)(y - wy - hy),
        (Sint16)(y + wy - hy),
        (Sint16)(y + wy + hx),
        (Sint16)(y - wy + hx)
    };

    filledPolygonRGBA(renderer, vx, vy, 4, color.r, color.g, color.b, color.a);
}

void drawLine(SDL_Renderer *renderer, float x1, float y1, float x2, float y2, SDL_Color color) {
    if (!renderer) return;
    aalineRGBA(renderer, (Sint16)x1, (Sint16)y1, (Sint16)x2, (Sint16)y2, color.r, color.g, color.b, color.a);
}

void circle(SDL_Renderer *renderer, float x, float y, float radius, SDL_Color color) {
    if (!renderer) return;
    aacircleRGBA(renderer, (Sint16)x, (Sint16)y, (Sint16)radius, color.r, color.g, color.b, color.a);
}

void filledCircle(SDL_Renderer *renderer, float x, float y, float radius, SDL_Color color) {
    if (!renderer) return;
    filledCircleRGBA(renderer, (Sint16)x, (Sint16)y, (Sint16)radius, color.r, color.g, color.b, color.a);
}

SDL_Surface *CreateSurfaceFromMemory(const unsigned char *data, size_t size) {
    if (!data || size == 0) return NULL;

    SDL_IOStream *rw = SDL_IOFromMem((void *)data, size);
    if (!rw) {
        SDL_Log("Failed to create IO stream: %s", SDL_GetError());
        return NULL;
    }

    SDL_Surface *surface = IMG_Load_IO(rw, 1);  // Auto-closes rw
    if (!surface) SDL_Log("Failed to load image from memory: %s", SDL_GetError());
    return surface;
}

void drawGrid(SDL_Renderer *renderer, Camera *camera, int w, int h) {
    if (!renderer || !camera || w <= 0 || h <= 0) return;

    SDL_SetRenderDrawColor(renderer, 44, 44, 44, 255);
    int step = 50;
    int start_x = ((int)camera->x / step) * step;
    int start_y = ((int)camera->y / step) * step;
    int end_x = camera->x + w, end_y = camera->y + h;

    for (int x = start_x; x <= end_x; x += step) {
        SDL_RenderLine(renderer, x - camera->x, 0, x - camera->x, h);
    }
    for (int y = start_y; y <= end_y; y += step) {
        SDL_RenderLine(renderer, 0, y - camera->y, w, y - camera->y);
    }
}

void drawBackground(SDL_Renderer *renderer, Camera *camera, int w, int h) {
    if (!renderer || !camera || w <= 0 || h <= 0) return;

    int step = 50;
    int start_x = ((int)camera->x / step) * step - (camera->x < 0 ? step : 0);
    int start_y = ((int)camera->y / step) * step - (camera->y < 0 ? step : 0);
    int end_x = camera->x + w, end_y = camera->y + h;

    for (int x = start_x; x <= end_x; x += step) {
        for (int y = start_y; y <= end_y; y += step) {
            SDL_SetRenderDrawColor(renderer, ((x / step + y / step) % 2) ? 132 : 126, ((x / step + y / step) % 2) ? 205 : 200, 0, 255);
            SDL_FRect rect = {x - camera->x, y - camera->y, step, step};
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

SDL_Color RGBA(int r, int g, int b, int a) {
    return (SDL_Color){(Uint8)r, (Uint8)g, (Uint8)b, (Uint8)a};
}

void drawText(void *data, const char *text, TTF_Font *font, float x, float y, SDL_Color color, bool centered) {
    if (!data || !text || !*text || !font) return;

    AppState *as = (AppState *)data;
    if (!as->renderer) return;

    SDL_Surface *surface = TTF_RenderText_Blended(font, text, 0, color);
    if (!surface) return;

    SDL_Texture *texture = SDL_CreateTextureFromSurface(as->renderer, surface);
    SDL_DestroySurface(surface);
    if (!texture) return;

    float tw, th;
    SDL_GetTextureSize(texture, &tw, &th);
    SDL_FRect rect = {centered ? x - tw * 0.5f : x, centered ? y - th * 0.5f : y, (float)tw, (float)th};
    SDL_RenderTexture(as->renderer, texture, NULL, &rect);
    SDL_DestroyTexture(texture);
}

void drawTextWrapped(void *data, const char *text, TTF_Font *font, float x, float y, SDL_Color color, int wrap_width, bool centered) {
    if (!data || !text || !*text || !font || wrap_width <= 0) return;

    AppState *as = (AppState *)data;
    if (!as->renderer) return;

    SDL_Surface *surface = TTF_RenderText_Blended_Wrapped(font, text, 0, color, wrap_width);
    if (!surface) return;

    SDL_Texture *texture = SDL_CreateTextureFromSurface(as->renderer, surface);
    SDL_DestroySurface(surface);
    if (!texture) return;

    float tw, th;
    SDL_GetTextureSize(texture, &tw, &th);
    SDL_FRect rect = {centered ? x - tw * 0.5f : x, centered ? y - th * 0.5f : y, (float)tw, (float)th};
    SDL_RenderTexture(as->renderer, texture, NULL, &rect);
    SDL_DestroyTexture(texture);
}

Point bezier_quadratic(Point p0, Point p1, Point p2, float t) {
    float u = 1 - t, u2 = u * u, t2 = t * t, ut = 2 * u * t;
    return (Point){
        .x = u2 * p0.x + ut * p1.x + t2 * p2.x,
        .y = u2 * p0.y + ut * p1.y + t2 * p2.y
    };
}