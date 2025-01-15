#include "../include/appstate.h"
#include "../include/enemy.h"

void updateEnemies(Enemy * enemies, Player * player, float dt) {
    int speed = 100;
    float dx = 0, dy = 0;

    if(enemies->x < player->x-25) {
        dx += speed * dt;
    } else {
        dx -= speed * dt;
    }

    if(enemies->y < player->y-25) {
        dy += speed * dt;
    } else {
        dy -= speed * dt;
    }

    float magnitude = SDL_sqrtf(dx * dx + dy * dy);
    if (magnitude > 0) {
        dx = (dx / magnitude) * speed * dt; // Normalise et applique la vitesse
        dy = (dy / magnitude) * speed * dt; // Normalise et applique la vitesse
    }

    enemies->x += dx;
    enemies->y += dy;
}

int enemyUpdateThread(void *data) {
    AppState *as = (AppState *)data;
    Uint64 last = 0;

    while (as->running) {
        Uint64 dt_ns;
        Uint64 now = SDL_GetTicksNS();
        // Lire la valeur de dt_ns
        SDL_LockMutex(as->dt_Mutex);
        dt_ns = as->dt_ns;
        SDL_UnlockMutex(as->dt_Mutex);

        // Convertir dt_ns en secondes
        float dt = dt_ns / 1e9f;

        static float im_w, im_h;

        if(!im_w || !im_h) {
            SDL_GetTextureSize(as->texture, &im_w, &im_h);
        }

        // Mettre à jour les ennemis
        SDL_LockMutex(as->enemyMutex);
        for (int i = 0; i < 32; i++) {
            updateEnemies(&as->enemies[i], as->player, dt);
        }
        SDL_UnlockMutex(as->enemyMutex);

        SDL_Delay(16);
    }

    return 0;
}