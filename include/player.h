#ifndef PLAYER_H
#define PLAYER_H

#include <SDL3/SDL.h>

#include "camera.h"

typedef struct {
    SDL_MouseID mouse;
    SDL_KeyboardID keyboard;
    float x;
    float y;
    float level;
    unsigned char zqsd;
} Player;

void initPlayers(Player * player);
void updatePlayer(Player * player, float dt);
void drawPlayer(SDL_Renderer *renderer, Player * player, Camera * camera, int w, int h);

#endif