#ifndef ENEMY_H
#define ENEMY_H

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

typedef struct {
    int type;
    float x, y;
    int w, h;
    SDL_Texture *image_texture;
    float speed;
    bool active;
    float reward;
} Enemy;

void initEnemies(Enemy *enemies, int count);
void generateEnemies(void *data, int speed, int reward, int count);
int enemyUpdateThread(void *data);
void drawEnemies(void *data);
Enemy *findClosestEnemy(void *data);

#endif