#include <SDL3/SDL.h>
#include <math.h>
#include <stdlib.h>

#include "../include/projectile.h"
#include "../include/appstate.h"
#include "../include/utils.h"

#include "../assets/archer/projectiles/arrow_cold.h"
#include "../assets/archer/projectiles/arrow_explosive.h"
#include "../assets/archer/projectiles/arrow_fire.h"
#include "../assets/archer/projectiles/arrow.h"

void initProjectile(Projectile * projectile, float x, float y, float dx, float dy, float angle) {
    *projectile = (Projectile){ .x = x, .y = y, .dx = dx, .dy = dy, .angle = angle, .active = true };
}

void updateProjectiles(void *appstate, int screen_w, int screen_h, float dt) {
    AppState *as = appstate;
    const int speed = 500;

    float projectile_im_w, projectile_im_h;
    float enemy_im_w, enemy_im_h;

    SDL_Surface* image_surface = CreateSurfaceFromMemory(arrow_png, arrow_png_len);
    SDL_Texture *image_texture = SDL_CreateTextureFromSurface(as->renderer, image_surface);
    SDL_DestroySurface(image_surface);
    SDL_GetTextureSize(image_texture, &projectile_im_w, &projectile_im_h);
    SDL_GetTextureSize(as->texture, &enemy_im_w, &enemy_im_h);

    for(int i = 0; i < as->projectile_number; i++) {
        if(as->projectiles[i].active) {
            float projectile_im_w, projectile_im_h;
            SDL_Surface* image_surface = CreateSurfaceFromMemory(arrow_png, arrow_png_len);
            SDL_Texture *image_texture = SDL_CreateTextureFromSurface(as->renderer, image_surface);
            SDL_DestroySurface(image_surface);
            SDL_GetTextureSize(image_texture, &projectile_im_w, &projectile_im_h);

            as->projectiles[i].x += as->projectiles[i].dx * speed * dt;
            as->projectiles[i].y += as->projectiles[i].dy * speed * dt;

            if(as->projectiles[i].x > screen_w || as->projectiles[i].x < 0) {
                as->projectiles[i].active = false;
                continue;
            }

            if(as->projectiles[i].y > screen_h || as->projectiles[i].y < 0) {
                as->projectiles[i].active = false;
                continue;
            }

            SDL_FRect projectile_rect;
            projectile_rect.x = as->projectiles[i].x+10;             
            projectile_rect.y = as->projectiles[i].y+10;             
            projectile_rect.w = projectile_im_w/6;         
            projectile_rect.h = projectile_im_h/6;

            for(int j = 0; j < as->enemy_number; j++) {
                if(as->enemies[j].active) {
                    float enemy_im_w, enemy_im_h;
                    SDL_GetTextureSize(as->texture, &enemy_im_w, &enemy_im_h);
                    SDL_FRect enemy_rect;
                    enemy_rect.x = as->enemies[j].x-enemy_im_w/2 - as->camera->x;             
                    enemy_rect.y = as->enemies[j].y-enemy_im_h/2 - as->camera->y;             
                    enemy_rect.w = enemy_im_w;         
                    enemy_rect.h = enemy_im_h;

                    if(SDL_HasRectIntersectionFloat(&projectile_rect, &enemy_rect)) {
                        as->projectiles[i].active = false;
                        as->enemies[j].active = false;  
                        break;          
                    }
                }
            }
        }
    }
}

void drawProjectiles(void *appstate) {
    AppState *as = appstate;

    SDL_SetRenderDrawColor(as->renderer, 255, 0, 0, 255);

    static SDL_Texture *projectile_texture = NULL;
    static float im_w = 0, im_h = 0;

    if (!projectile_texture) {
        SDL_Surface *image_surface = CreateSurfaceFromMemory(arrow_png, arrow_png_len);
        projectile_texture = SDL_CreateTextureFromSurface(as->renderer, image_surface);
        SDL_DestroySurface(image_surface);
        SDL_GetTextureSize(projectile_texture, &im_w, &im_h);
    }

    for(int i = 0; i < as->projectile_number; i++) {
        Projectile *proj = &as->projectiles[i];
        if (!proj->active) continue;
            
        SDL_FRect dest_rect = {
            .x = proj->x + 10,
            .y = proj->y + 10,
            .w = im_w / 6,
            .h = im_h / 6
        };

        SDL_RenderTextureRotated(as->renderer, projectile_texture, NULL, &dest_rect, proj->angle * 180/M_PI, NULL, SDL_FLIP_NONE);
    }
}