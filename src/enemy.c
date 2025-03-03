#include <float.h>

#include "../include/appstate.h"
#include "../include/enemy.h"
#include "../include/utils.h"

/* #include "../assets/Sprite-WhiteHand.h"
#include "../assets/boss/el_diablo.h" */

#define TIME_SCALE 1.0e-9f
#define ENEMY_SPEED 100.0f
#define PUSH_FACTOR 100.0f
#define MIN_DISTANCE 50.0f

static inline SDL_FRect getEnemyRect(const Enemy *enemy, const Camera *camera) {
    return (SDL_FRect){
        .x = enemy->x - enemy->w / 2 - camera->x,
        .y = enemy->y - enemy->h / 2 - camera->y,
        .w = enemy->w,
        .h = enemy->h
    };
}

void initEnemies(Enemy *enemies, int count) {
    for (int i = 0; i < count; i++) {
        enemies[i] = (Enemy){
            .type = 0,
            .active = true,
            .x = (float)SDL_rand(300),
            .y = (float)SDL_rand(300),
            .w = 40.0f,  // Default size instead of 0
            .h = 40.0f,
            .speed = 25.0f,
            .reward = 20,
            .image_texture = NULL
        };
    }
};

void generateEnemies(void *data, int speed, int reward, int count) {
    AppState *as = (AppState *)data;

    if(as->game_over) return;

    static SDL_Texture *ennemi = NULL;
    static SDL_Texture *ennemi2 = NULL;

/*     if (!ennemi || !ennemi2) {
        SDL_Surface *image_surface = CreateSurfaceFromMemory(Sprite_WhiteHand_png, Sprite_WhiteHand_png_len);
        ennemi = SDL_CreateTextureFromSurface(as->renderer, image_surface);
        SDL_DestroySurface(image_surface);

        SDL_Surface *image_surface2 = CreateSurfaceFromMemory(el_diablo_png, el_diablo_png_len);
        ennemi2 = SDL_CreateTextureFromSurface(as->renderer, image_surface2);
        SDL_DestroySurface(image_surface2);
    } */

    if(as->is_paused || as->upgrade_menu || count == 0) return;
    
    SDL_LockMutex(as->enemyMutex);
    int screen_w, screen_h;
    if (!SDL_GetRenderOutputSize(as->renderer, &screen_w, &screen_h)) {
        SDL_UnlockMutex(as->enemyMutex);
        return;
    }
    
    for (int i = 0; i < count; i++) {
        int side = SDL_rand(4); // 0: haut, 1: bas, 2: gauche, 3: droite
        int x, y;
        
        switch (side) {
            case 0: x = SDL_rand(screen_w); y = -50; break;
            case 1: x = SDL_rand(screen_w); y = screen_h + 50; break;
            case 2: x = -50; y = SDL_rand(screen_h); break;
            case 3: x = screen_w + 50; y = SDL_rand(screen_h); break;
        }

        Enemy *enemies = as->enemies;

        // Ajouter l'ennemi dans la liste
        for (int j = 0; j < as->enemy_number; j++) {
            if (!enemies[j].active) {
                int type = SDL_rand(3);
                enemies[j] = (Enemy){
                    .active = true,
                    .x = x + as->camera->x,
                    .y = y + as->camera->y,
                    .type = type,
                    .speed = speed,
                    .reward = reward,
                    .image_texture = NULL
                };

                enemies[j].image_texture = (type == 1) ? ennemi2 : ennemi;

                float tex_w, tex_h;
                SDL_GetTextureSize(enemies[j].image_texture, &tex_w, &tex_h);
                enemies[j].w = 100;
                enemies[j].h = 100;

                as->current_enemy_number++;
                break;
            }
        }
    }
    SDL_UnlockMutex(as->enemyMutex);
}

static void updateEnemies(AppState *as, float dt) {
    if(as->game_over) return;

    Player *player = as->player;
    Enemy *enemies = as->enemies;
    Camera *camera = as->camera;

    const float push_factor = 100.0f;
    const float min_distance = 50.0f;

    float dx = 0, dy = 0;

    int w, h;
    if (!SDL_GetRenderOutputSize(as->renderer, &w, &h)) return;

    SDL_FRect player_rect = {
        .x = w / 2 - player->w / 2,
        .y = h / 2 - player->h / 2,
        .w = player->w,
        .h = player->h
    };

    for(int i = 0; i < as->enemy_number; i++) {
        if (!enemies[i].active) continue;

        float dx = (enemies[i].x < player->x) ? enemies[i].speed * dt : -enemies[i].speed * dt;
        float dy = (enemies[i].y < player->y) ? enemies[i].speed * dt : -enemies[i].speed * dt;
        float magnitude = SDL_sqrtf(dx * dx + dy * dy);

        if (magnitude > 0.0f) {
            dx = (dx / magnitude) * enemies[i].speed * dt;
            dy = (dy / magnitude) * enemies[i].speed * dt;
        }

        SDL_FRect enemy_rect = getEnemyRect(&enemies[i], camera);
        bool collision_with_player = SDL_HasRectIntersectionFloat(&player_rect, &enemy_rect);

        bool collision_with_other_enemies = false;
        float cumulative_repulsion_dx = 0.0f;
        float cumulative_repulsion_dy = 0.0f;

        for (int j = 0; j < as->enemy_number; j++) {
            if (i == j || !enemies[j].active) continue;

            SDL_FRect enemy2_rect = {
                .x = enemies[j].x - enemies[i].w / 2 - camera->x,
                .y = enemies[j].y - enemies[i].h / 2 - camera->y,
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

    Uint64 last_spawn_time = 0, past = 0;

    while (SDL_GetAtomicInt(&as->running)) {
        Uint64 now = SDL_GetTicksNS();
        float dt = (now - past) / 1e9f;

        SDL_LockMutex(as->enemyMutex);
        if(!as->is_paused && !as->upgrade_menu && as->page == 1) {
            updateEnemies(as, dt);
            if ((now - last_spawn_time) * TIME_SCALE >= 3.0f) {
                generateEnemies(as, 50, 10, (as->player->level < 10) ? 1 : (int)as->player->level / 10);
                last_spawn_time = now;
            }
        }
        SDL_UnlockMutex(as->enemyMutex);

        past = now;
        Uint64 elapsed = SDL_GetTicksNS() - now;
        if (elapsed < 999999) {
            SDL_DelayNS(999999 - elapsed);
        }
    }
    return 0;
}

void drawEnemies(void *data) {
    AppState *as = (AppState *)data;
    
    Camera *camera = as->camera;
    Enemy *enemies = as->enemies;
    SDL_Renderer *renderer = as->renderer;

    int w, h;
    if (!SDL_GetRenderOutputSize(renderer, &w, &h)) return;

    SDL_LockMutex(as->enemyMutex);
    for(int i = 0; i < as->enemy_number; i++) {
        if(enemies[i].active) {
            if(as->enemies[i].x - as->camera->x < w + 50 && as->enemies[i].x - as->camera->x > -50 ) {
                if(as->enemies[i].y - as->camera->y < h + 50 && as->enemies[i].y - as->camera->y > -50) {
                    SDL_FRect dest_rect = getEnemyRect(&as->enemies[i], as->camera);
                    dest_rect.w = enemies[i].w;
                    dest_rect.h = enemies[i].h;
                    SDL_RenderTexture(renderer, (enemies[i].type == 0) ? as->texture : as->texture2, NULL, &dest_rect);
                    
                    if(as->debug_mode) {
                        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 100);
                        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                        SDL_RenderFillRect(renderer, &dest_rect);
                        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
                    }
                }
            } 
        }
    }
    SDL_UnlockMutex(as->enemyMutex);
}

Enemy *findClosestEnemy(void *data) {
    AppState *as = (AppState *)data;

    if (as->current_enemy_number == 0) return NULL;

    SDL_LockMutex(as->enemyMutex);
    Enemy *closest = NULL;
    float min_distance = FLT_MAX;

    for (int i = 0; i < as->enemy_number; i++) {
        if (!as->enemies[i].active) continue;
        float dx = as->enemies[i].x - as->player->x;
        float dy = as->enemies[i].y - as->player->y;
        float distance = dx * dx + dy * dy;
        if (distance < min_distance) {
            min_distance = distance;
            closest = &as->enemies[i];
        }
    }
    SDL_UnlockMutex(as->enemyMutex);
    return closest;
}