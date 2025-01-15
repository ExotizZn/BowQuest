#include <SDL3/SDL.h>
#include <math.h>

#include "../include/projectile.h"
#include "../include/utils.h"

#include "../assets/archer/projectiles/arrow_cold.h"
#include "../assets/archer/projectiles/arrow_explosive.h"
#include "../assets/archer/projectiles/arrow_fire.h"
#include "../assets/archer/projectiles/arrow.h"

Projectile * initProjectile(float x, float y, float dx, float dy) {
    Projectile * new_projectile = SDL_malloc(sizeof(Projectile)) ;

    new_projectile->x = x;
    new_projectile->y = y;
    new_projectile->dx = dx;
    new_projectile->dy = dy;

    return new_projectile;
}

void updateProjectiles(Projectile * projectiles, int projectile_number, int screen_w, int screen_h, float dt) {
    int speed = 500;

    for(int i = 0; i < projectile_number; i++) {
        projectiles[i].x += projectiles[i].dx * speed * dt;
        projectiles[i].y += projectiles[i].dy * speed * dt;
    }
}

void drawProjectiles(SDL_Renderer *renderer, Camera * camera, Projectile * projectiles, int projectile_number) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    static float im_w, im_h;

    SDL_Surface* image_surface = CreateSurfaceFromMemory(arrow_png, arrow_png_len);
    SDL_Texture *image_texture = SDL_CreateTextureFromSurface(renderer, image_surface);
    SDL_DestroySurface(image_surface);
    SDL_GetTextureSize(image_texture, &im_w, &im_h);

    int w, h;
    if (!SDL_GetRenderOutputSize(renderer, &w, &h)) {
        return;
    }

    for(int i = 0; i < projectile_number; i++) {
        SDL_FRect dest_rect;
        dest_rect.x = projectiles[i].x+10;             
        dest_rect.y = projectiles[i].y+10;             
        dest_rect.w = im_w/6;         
        dest_rect.h = im_h/6;

        SDL_RenderTextureRotated(renderer, image_texture, NULL, &dest_rect, projectiles[i].angle * 180/M_PI, NULL, SDL_FLIP_NONE);
    }
}