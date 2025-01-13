#include "../include/enemy.h"

void updateEnemies(Enemy * enemies, Player * player, float dt) {
    int speed = 50;
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