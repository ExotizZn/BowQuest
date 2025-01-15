#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "SDL3/SDL.h"

#include "camera.h"

typedef struct {
    float x, y;
    float dx, dy;
    float angle;
} Projectile;

Projectile * initProjectile(float x, float y, float dx, float dy);
void updateProjectiles(Projectile * projectiles, int projectile_number, int screen_w, int screen_h, float dt);
void drawProjectiles(SDL_Renderer *renderer, Camera * camera, Projectile * projectiles, int projectile_number);

#endif