#include "../include/appstate.h"
#include "../include/enemy.h"

void initEnemies(Enemy * enemies, int count) {
    for (int i = 0; i < count; i++) {
        enemies[i].active = true;
        enemies[i].x = SDL_rand(300);
        enemies[i].y = SDL_rand(300);
        enemies[i].w = 100;
        enemies[i].h = 100;
    }
}

static void updateEnemies(Enemy * enemies, const Player * player, int enemy_count, float dt) {
    const float speed = 100.0f;

    float dx = 0, dy = 0;

    const float player_x_center = player->x - 25.0f;
    const float player_y_center = player->y - 25.0f;

    for(int i = 0; i < enemy_count; i++) {
        if (!enemies[i].active) continue; 

        float dx = (enemies[i].x < player_x_center) ? speed * dt : -speed * dt;
        float dy = (enemies[i].y < player_y_center) ? speed * dt : -speed * dt;

        float magnitude = SDL_sqrtf(dx * dx + dy * dy);
        if (magnitude > 0.0f) {
            dx = (dx / magnitude) * speed * dt;
            dy = (dy / magnitude) * speed * dt;
        }

        enemies[i].x += dx;
        enemies[i].y += dy;
    }
}

int enemyUpdateThread(void *data) {
    AppState *as = (AppState *)data;

    while (SDL_GetAtomicInt(&as->running)) {
        float dt;
        // Lire la valeur de dt_ns
        SDL_LockMutex(as->dt_Mutex);
        dt = as->dt_ns / 1e9f;
        SDL_UnlockMutex(as->dt_Mutex);

        // Mettre à jour les ennemis
        SDL_LockMutex(as->enemyMutex);
        updateEnemies(as->enemies, as->player, as->enemy_number, dt);
        SDL_UnlockMutex(as->enemyMutex);

        SDL_Delay(16);
    }

    return 0;
}

void drawEnemies(void *data) {
    AppState * as = (AppState *)data;

    float im_w, im_h;
    SDL_GetTextureSize(as->texture, &im_w, &im_h);

    SDL_LockMutex(as->enemyMutex);
    for(int i = 0; i < as->enemy_number; i++) {
        if(as->enemies[i].active) {
            SDL_FRect dest_rect = {
                .x = as->enemies[i].x-im_w/2 - as->camera->x,      
                .y = as->enemies[i].y-im_h/2 - as->camera->y,
                .w = im_w,    
                .h = im_h
            };

            SDL_RenderTexture(as->renderer, as->texture, NULL, &dest_rect);
        }
    }
    SDL_UnlockMutex(as->enemyMutex);
}