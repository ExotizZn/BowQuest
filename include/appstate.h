#ifndef APPSTATE_H
#define APPSTATE_H

#include <stdbool.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "camera.h"
#include "enemy.h"
#include "fonts.h"
#include "player.h"
#include "projectile.h"

typedef struct AppState {
    SDL_Window *window;
    SDL_Renderer *renderer;
    Camera *camera;
    Player *player;
    Projectile * projectiles;
    Enemy * enemies;
    SDL_Thread *enemyThread;
    SDL_Mutex *enemyMutex;
    Uint64 dt_ns;
    SDL_AtomicInt running;
    SDL_Texture * texture;
    SDL_Texture ** skills_assets;
    int projectile_number;
    int current_enemy_number;
    int enemy_number;
    int page;
    Fonts * fonts;
    bool is_paused;
    bool upgrade_menu;
    bool debug_mode;
} AppState;

#endif