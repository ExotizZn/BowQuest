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

void addProjectile(void *appstate, float target_x, float target_y);
void addProjectileDebugMode(void *appstate, float target_x, float target_y, int w, int h);
void updateProjectiles(void *appstate, int screen_w, int screen_h, float dt);
void drawProjectiles(void *appstate);
void cleanupProjectileTexture(void);

#endif