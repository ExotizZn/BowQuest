#ifndef APPSTATE_H
#define APPSTATE_H

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "player.h"
#include "enemy.h"
#include "projectile.h"
#include "camera.h"

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    Camera *camera;
    Player *player;
    Projectile * projectiles;
    Enemy * enemies;
    SDL_Thread *enemyThread;
    SDL_Mutex *enemyMutex;
    SDL_Mutex *dt_Mutex;
    Uint64 dt_ns;
    int running;
    SDL_Texture * texture;
    int projectile_number;
    int enemy_number;
} AppState;

#endif