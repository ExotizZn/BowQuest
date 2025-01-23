#ifndef ENEMY_H
#define ENEMY_H

#include <SDL3/SDL.h>

#include "player.h"

typedef struct {
    float x, y;
    int w, h;
    SDL_Texture * image_texture;
    bool active;
} Enemy;

void initEnemies(Enemy * enemies, int count);
int enemyUpdateThread(void *data);
void drawEnemies(void *data);

#endif