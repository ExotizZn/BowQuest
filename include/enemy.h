#ifndef ENEMY_H
#define ENEMY_H

#include <SDL3/SDL.h>

#include "player.h"

typedef struct {
    float x, y;
    int w, h;
    SDL_Texture * image_texture;
} Enemy;

void updateEnemies(Enemy * enemies, Player * player, float dt);

#endif