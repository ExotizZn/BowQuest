#ifndef PLAYER_H
#define PLAYER_H

#include <SDL3/SDL.h>

#include "camera.h"
#include "enemy.h"

typedef struct {
    float projectile_delay;
    float projectile_speed;
    float projectile_damage;
    float crit_chance;
    float armor;
    float regen;
} Player_Stats;

typedef struct {
    SDL_MouseID mouse;
    SDL_KeyboardID keyboard;
    float x;
    float y;
    float w;
    float h;
    int level;
    float speed;
    float progression_to_next_level;
    int health;
    unsigned char zqsd;
    Player_Stats * player_stats;
} Player;

// Fonction pour initialiser le joueur
void initPlayers(Player * player);

// Fonction pour mettre à jour le joueur
void updatePlayer(void * data, float dt);

// Fonction pour afficher le joueur
void drawPlayer(void * data);

#endif