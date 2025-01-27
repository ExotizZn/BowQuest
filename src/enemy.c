#include "../include/appstate.h"
#include "../include/enemy.h"

void initEnemies(Enemy * enemies, int count) {
    for (int i = 0; i < count; i++) {
        enemies[i].active = true;
        enemies[i].x = SDL_rand(300);
        enemies[i].y = SDL_rand(300);
        enemies[i].w = 0;
        enemies[i].h = 0;
        enemies[i].reward = 20;
    }
}

static void updateEnemies(AppState * as, float dt) {
    Player * player = as->player;
    Enemy * enemies = as->enemies;
    Camera * camera = as->camera;

    const float speed = 100.0f;
    const float push_factor = 100.0f;
    const float min_distance = 50.0f;

    float dx = 0, dy = 0;

    int w, h;
    if (!SDL_GetRenderOutputSize(as->renderer, &w, &h)) {
        return;
    }

    SDL_FRect player_rect = {
        .x = w/2 - player->w/8,
        .y = h/2 - player->h/8,
        .w = player->w / 4,
        .h = player->h / 4
    };

    for(int i = 0; i < as->enemy_number; i++) {
        if (!enemies[i].active) continue; 

        float dx = (enemies[i].x < player->x) ? speed * dt : -speed * dt;
        float dy = (enemies[i].y < player->y) ? speed * dt : -speed * dt;

        float magnitude = SDL_sqrtf(dx * dx + dy * dy);
        if (magnitude > 0.0f) {
            dx = (dx / magnitude) * speed * dt;
            dy = (dy / magnitude) * speed * dt;
        }

        SDL_FRect enemy_rect = {
            .x = enemies[i].x - enemies[i].w/2 - camera->x,      
            .y = enemies[i].y - enemies[i].h/2 - camera->y,
            .w = enemies[i].w,    
            .h = enemies[i].h
        };

        bool collision_with_player = SDL_HasRectIntersectionFloat(&player_rect, &enemy_rect);
        bool collision_with_other_enemies = false;

        float cumulative_repulsion_dx = 0.0f;
        float cumulative_repulsion_dy = 0.0f;

        for (int j = 0; j < as->enemy_number; j++) {
            if (i == j || !enemies[j].active) continue;

            SDL_FRect enemy2_rect = {
                .x = enemies[j].x - enemies[i].w/2 - camera->x,      
                .y = enemies[j].y - enemies[i].h/2 - camera->y,
                .w = enemies[j].w,    
                .h = enemies[j].h
            };

            if (SDL_HasRectIntersectionFloat(&enemy_rect, &enemy2_rect)) {
                collision_with_other_enemies = true;

                float repulsion_dx = enemies[i].x - enemies[j].x;
                float repulsion_dy = enemies[i].y - enemies[j].y;
                float repulsion_magnitude = SDL_sqrtf(repulsion_dx * repulsion_dx + repulsion_dy * repulsion_dy);

                // Appliquer la répulsion si nécessaire
                if (repulsion_magnitude > 0.0f) {
                    // Normaliser le vecteur de répulsion et l'appliquer aux ennemis
                    repulsion_dx /= repulsion_magnitude;
                    repulsion_dy /= repulsion_magnitude;

                    enemies[i].x += repulsion_dx * push_factor * dt;
                    enemies[i].y += repulsion_dy * push_factor * dt;
                    enemies[j].x -= repulsion_dx * push_factor * dt;
                    enemies[j].y -= repulsion_dy * push_factor * dt;
                }

                break;
            }
        }

        if (!collision_with_player && !collision_with_other_enemies) {
            enemies[i].x += dx;
            enemies[i].y += dy;
        }
    }
}

int enemyUpdateThread(void *data) {
    AppState *as = (AppState *)data;

    static Uint64 last = 0;
    static Uint64 past = 0;

    while (SDL_GetAtomicInt(&as->running)) {
        Uint64 now = SDL_GetTicksNS();
        float dt = (now - past) / 1e9f;

        if(!as->is_paused) {
            SDL_LockMutex(as->enemyMutex);
            updateEnemies(as, dt);
            SDL_UnlockMutex(as->enemyMutex);
        }

        past = now;
        Uint64 elapsed = SDL_GetTicksNS() - now;
        if (elapsed < 999999) {
            SDL_DelayNS(999999 - elapsed);
        }
    }

    return 0;
}

void drawEnemies(void *data) {
    float im_w, im_h;
    AppState * as = (AppState *)data;

    SDL_GetTextureSize(as->texture, &im_w, &im_h);
    
    Camera * camera = as->camera;
    Enemy * enemies = as->enemies;
    SDL_Renderer * renderer = as->renderer;

    SDL_LockMutex(as->enemyMutex);
    for(int i = 0; i < as->enemy_number; i++) {
        if(enemies[i].active) {
            SDL_FRect dest_rect = {
                .x = enemies[i].x-im_w/2 - camera->x,      
                .y = enemies[i].y-im_h/2 - camera->y,
                .w = im_w,    
                .h = im_h
            };

            SDL_RenderTexture(renderer, as->texture, NULL, &dest_rect);
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 100);

            if(as->debug_mode) {
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                SDL_RenderFillRect(renderer, &dest_rect);
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
            }
        }
    }
    SDL_UnlockMutex(as->enemyMutex);
}