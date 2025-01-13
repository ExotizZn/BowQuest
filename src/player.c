#include "../include/player.h"
#include "../include/utils.h"

#include "../assets/archer/archer_left.h"
#include "../assets/archer/archer_left_down.h"
#include "../assets/archer/archer_left_up.h"

#include "../assets/archer/archer_right.h"
#include "../assets/archer/archer_right_down.h"
#include "../assets/archer/archer_right_up.h"

void initPlayers(Player * player) {
    player->x = 0;
    player->y = 0;
    player->level = 0;
    player->zqsd = 0;
    player->mouse = 0;
    player->keyboard = 0;
}

void updatePlayer(Player * player, float dt) {
    int speed = 500;
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
}

void drawPlayer(SDL_Renderer *renderer, Player * player, Camera * camera, int w, int h) {
    static float im_w, im_h;
    SDL_Surface* image_surface = NULL;
    static SDL_Texture *image_texture = NULL;

    // Z (haut)
    if (player->zqsd & 0x01) {
        image_surface = CreateSurfaceFromMemory(archer_left_up_png, archer_left_up_png_len);
        image_texture = SDL_CreateTextureFromSurface(renderer, image_surface);
        SDL_DestroySurface(image_surface);
        SDL_GetTextureSize(image_texture, &im_w, &im_h);
    } 
    // Q (gauche)
    if (player->zqsd & 0x02) {
        image_surface = CreateSurfaceFromMemory(archer_left_png, archer_left_png_len);
        image_texture = SDL_CreateTextureFromSurface(renderer, image_surface);
        SDL_DestroySurface(image_surface);
        SDL_GetTextureSize(image_texture, &im_w, &im_h); 
    }

    // S (bas)
    if (player->zqsd & 0x04) {
        image_surface = CreateSurfaceFromMemory(archer_left_down_png, archer_left_down_png_len);
        image_texture = SDL_CreateTextureFromSurface(renderer, image_surface);
        SDL_DestroySurface(image_surface);
        SDL_GetTextureSize(image_texture, &im_w, &im_h); 
    }
    // D (droite)
    if (player->zqsd & 0x08) {
        image_surface = CreateSurfaceFromMemory(archer_right_png, archer_right_png_len);
        image_texture = SDL_CreateTextureFromSurface(renderer, image_surface);
        SDL_DestroySurface(image_surface);
        SDL_GetTextureSize(image_texture, &im_w, &im_h); 
    }

    SDL_FRect dest_rect;
    dest_rect.x = player->x-im_w/8 - camera->x;             
    dest_rect.y = player->y-im_h/8 - camera->y;             
    dest_rect.w = im_w/4;         
    dest_rect.h = im_h/4;

    SDL_RenderTexture(renderer, image_texture, NULL, &dest_rect);
}