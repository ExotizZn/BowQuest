#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "SDL3/SDL.h"

#include "enemy.h"
#include "camera.h"

typedef struct {
    float x, y;
    float dx, dy;
    float angle;
    bool active;
} Projectile;

void initProjectile(Projectile * projectile, float x, float y, float dx, float dy, float angle);
void updateProjectiles(void *appstate, int screen_w, int screen_h, float dt);
void drawProjectiles(void *appstate);

#endif