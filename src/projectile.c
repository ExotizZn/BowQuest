#include <SDL3/SDL.h>
#include <math.h>
#include <stdlib.h>

#include "../include/projectile.h"
#include "../include/appstate.h"
#include "../include/player.h"
#include "../include/utils.h"

#include "../assets/archer/projectiles/arrow_cold.h"
#include "../assets/archer/projectiles/arrow_explosive.h"
#include "../assets/archer/projectiles/arrow_fire.h"
#include "../assets/archer/projectiles/arrow.h"

#define PROJECTILE_SPEED 1000.0f
#define DEG_PER_RAD (180.0f / M_PI)  // Precomputed for angle conversion
#define MAX_PROJECTILES 100          // Assuming this from earlier context

static void initProjectile(Projectile *projectile, float x, float y, float dx, float dy, float angle) {
    if (!projectile) return;
    *projectile = (Projectile) {
        .x = x,
        .y = y,
        .dx = dx,
        .dy = dy,
        .angle = angle,
        .active = true
    };
}

void addProjectile(void *appstate, float target_x, float target_y) {
    AppState *as = (AppState *)appstate;
    if (!as || !as->player || as->projectile_number >= MAX_PROJECTILES) return;

    Player *player = as->player;
    float angle = SDL_atan2f(target_y - player->y, target_x - player->x);
    float dx = SDL_cosf(angle), dy = SDL_sinf(angle);

    initProjectile(&as->projectiles[as->projectile_number++], player->x, player->y, dx, dy, angle);
}

void addProjectileDebugMode(void *appstate, float target_x, float target_y, int w, int h) {
    AppState *as = (AppState *)appstate;

    if(as->game_over) return;

    if (!as || !as->player || as->projectile_number >= MAX_PROJECTILES) return;

    Player *player = as->player;
    float angle = SDL_atan2f(target_y - h / 2, target_x - w / 2);
    float dx = SDL_cosf(angle), dy = SDL_sinf(angle);

    initProjectile(&as->projectiles[as->projectile_number++], player->x, player->y, dx, dy, angle);
}

void updateProjectiles(void *appstate, int screen_w, int screen_h, float dt) {
    AppState *as = (AppState *)appstate;

    if(as->game_over || as->page != 1) return;

    if (!as || !as->renderer || !as->camera || !as->enemies || !as->player) return;

    static float proj_w = 0, proj_h = 0;
    static bool sizes_loaded = false;
    if (!sizes_loaded) {
        SDL_Surface *surf = CreateSurfaceFromMemory(arrow_png, arrow_png_len);
        if (surf) {
            SDL_Texture *tex = SDL_CreateTextureFromSurface(as->renderer, surf);
            SDL_DestroySurface(surf);
            if (tex) {
                SDL_GetTextureSize(tex, &proj_w, &proj_h);
                SDL_DestroyTexture(tex);  // Temp texture for size
            }
        }

        sizes_loaded = true;
    }
    
    if (!proj_w) return;

    SDL_LockMutex(as->enemyMutex);  // Protect enemy access
    for (int i = 0; i < as->projectile_number; i++) {
        Projectile *proj = &as->projectiles[i];
        if (!proj->active) continue;

        proj->x += proj->dx * PROJECTILE_SPEED * dt;
        proj->y += proj->dy * PROJECTILE_SPEED * dt;

        float proj_x = proj->x - 20 - as->camera->x;
        float proj_y = proj->y - as->camera->y;
        if (proj_x > screen_w || proj_x < 0 || proj_y > screen_h || proj_y < 0) {
            proj->active = false;
            continue;
        }

        SDL_FRect proj_rect = {proj_x, proj_y, proj_w / 7, proj_h / 7};
        for (int j = 0; j < as->enemy_number; j++) {
            Enemy *enemy = &as->enemies[j];
            if (!enemy->active) continue;

            SDL_FRect enemy_rect = {
                .x = enemy->x - enemy->w / 2 - as->camera->x,
                .y = enemy->y - enemy->h / 2 - as->camera->y,
                .w = enemy->w,
                .h = enemy->h
            };

            if (SDL_HasRectIntersectionFloat(&proj_rect, &enemy_rect)) {
                proj->active = false;
                enemy->active = false;
                as->current_enemy_number--;
                as->player->progression_to_next_level = SDL_min(100.0f, as->player->progression_to_next_level + enemy->reward);
                addCoin(as, enemy->x - enemy->w / 2, enemy->y - enemy->h / 2);
                break;
            }
        }
    }
    SDL_UnlockMutex(as->enemyMutex);

    // Compact projectile array (optional, reduces future iterations)
    int write_idx = 0;
    for (int i = 0; i < as->projectile_number; i++) {
        if (as->projectiles[i].active) {
            if (write_idx != i) as->projectiles[write_idx] = as->projectiles[i];
            write_idx++;
        }
    }
    as->projectile_number = write_idx;
}

static SDL_Texture *proj_texture = NULL;
static float im_w = 0, im_h = 0;
static bool texture_loaded = false;

void cleanupProjectileTexture(void) {
    if (proj_texture) {
        SDL_DestroyTexture(proj_texture);
        proj_texture = NULL;
    }
    texture_loaded = false;
}

void drawProjectiles(void *appstate) {
    AppState *as = (AppState *)appstate;

    if(as->game_over) return;

    if (!as || !as->renderer || !as->camera) return;

    if (!texture_loaded) {
        SDL_Surface *surf = CreateSurfaceFromMemory(arrow_png, arrow_png_len);
        if (surf) {
            proj_texture = SDL_CreateTextureFromSurface(as->renderer, surf);
            SDL_DestroySurface(surf);
            if (proj_texture) SDL_GetTextureSize(proj_texture, &im_w, &im_h);
            else SDL_Log("Failed to load projectile texture: %s", SDL_GetError());
        }
        texture_loaded = true;
    }
    if (!proj_texture) return;

    for (int i = 0; i < as->projectile_number; i++) {
        Projectile *proj = &as->projectiles[i];
        if (!proj->active) continue;

        SDL_FRect dest_rect = {
            .x = proj->x - 20 - as->camera->x,
            .y = proj->y - as->camera->y,
            .w = im_w / 7,
            .h = im_h / 7
        };

        float angle_deg = proj->angle * DEG_PER_RAD;
        SDL_RenderTextureRotated(as->renderer, proj_texture, NULL, &dest_rect, angle_deg, NULL, SDL_FLIP_NONE);

        if (as->debug_mode) {
            SDL_SetRenderDrawColor(as->renderer, 255, 0, 255, 100);
            SDL_SetRenderDrawBlendMode(as->renderer, SDL_BLENDMODE_BLEND);
            drawFilledRotatedRect(as->renderer, proj->x + 6 - as->camera->x, proj->y + 6 - as->camera->y, 
                                 im_w / 6, im_h / 6, angle_deg, RGBA(255, 0, 255, 100));
            SDL_SetRenderDrawBlendMode(as->renderer, SDL_BLENDMODE_NONE);
        }
    }
}