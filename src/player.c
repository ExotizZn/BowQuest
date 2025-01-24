#include "../include/player.h"
#include "../include/utils.h"

#include "../assets/archer/archer_left.h"
#include "../assets/archer/archer_left_down.h"
#include "../assets/archer/archer_left_up.h"

void initPlayers(Player * player) {
    *player = (Player){
        .x = 0,
        .y = 0,
        .level = 0,
        .progression_to_next_level = 0,
        .health = 100,
        .zqsd = 0,
        .mouse = 0,
        .keyboard = 0
    };
}

void updatePlayer(Player * player, float dt) {
    const int speed = 500;
    float dx = 0, dy = 0;

    // Calcule les directions en fonction des touches enfoncées
    if (player->zqsd & 0x01) dy -= speed * dt; // Z (haut)
    if (player->zqsd & 0x02) dx -= speed * dt; // Q (gauche)
    if (player->zqsd & 0x04) dy += speed * dt; // S (bas)
    if (player->zqsd & 0x08) dx += speed * dt; // D (droite)

    // Calcule la magnitude uniquement si un déplacement est requis
    float magnitude = SDL_sqrtf(dx * dx + dy * dy);
    if (magnitude > 0) {
        dx = (dx / magnitude) * speed * dt; // Normalise et applique la vitesse
        dy = (dy / magnitude) * speed * dt; // Normalise et applique la vitesse
    }

    player->x += dx;
    player->y += dy;

    player->progression_to_next_level = 6.5;
/* 
    if(player->progression_to_next_level >= 100) {
        player->progression_to_next_level = 0;
        player->level ++;
    } */
}

void drawPlayer(SDL_Renderer *renderer, Player * player, Camera * camera, int w, int h) {
    static float im_w = 0, im_h = 0;
    static SDL_Texture *image_texture[3] = {NULL};

    if (!image_texture[0]) {
        image_texture[0] = SDL_CreateTextureFromSurface(renderer, CreateSurfaceFromMemory(archer_left_png, archer_left_png_len));
        image_texture[1] = SDL_CreateTextureFromSurface(renderer, CreateSurfaceFromMemory(archer_left_down_png, archer_left_down_png_len));
        image_texture[2] = SDL_CreateTextureFromSurface(renderer, CreateSurfaceFromMemory(archer_left_up_png, archer_left_up_png_len));
        SDL_GetTextureSize(image_texture[0], &im_w, &im_h);
    }

    SDL_Texture * current_texture = image_texture[0];
    static SDL_FlipMode flip_mode = SDL_FLIP_NONE;

    if (player->zqsd & 0x01) {
        current_texture = image_texture[2]; // Z (haut)
    } else if (player->zqsd & 0x04) {
        current_texture = image_texture[1]; // S (bas)
    }

    if (player->zqsd & 0x02) {
        flip_mode = SDL_FLIP_NONE; // Q (gauche)
    } else if (player->zqsd & 0x08) {
        flip_mode = SDL_FLIP_HORIZONTAL; // D (droite)
    }

    SDL_FRect dest_rect = {
        .x = player->x - im_w / 8 - camera->x,
        .y = player->y - im_h / 8 - camera->y,
        .w = im_w / 4,
        .h = im_h / 4
    };

    SDL_RenderTextureRotated(renderer, current_texture, NULL, &dest_rect, 0, NULL, flip_mode);

    SDL_SetRenderDrawColor(renderer, 22, 22, 22, 255); // Couleur du fond de la barre
    fillRect(renderer, w / 2 - 35, h / 2 - 80, 70, 10);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Couleur de la vie restante
    fillRect(renderer, w / 2 - 35, h / 2 - 80, (float)player->health / 100 * 70, 10);
}