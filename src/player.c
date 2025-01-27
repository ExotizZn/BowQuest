#include "../include/appstate.h"
#include "../include/player.h"
#include "../include/utils.h"

#include "../assets/archer/archer_left.h"
#include "../assets/archer/archer_left_down.h"
#include "../assets/archer/archer_left_up.h"

void initPlayers(Player * player) {
    *player = (Player){
        .x = 1000,
        .y = 1000,
        .w = 0,
        .h = 0,
        .level = 0,
        .speed = 200,
        .progression_to_next_level = 0,
        .health = 100,
        .zqsd = 0,
        .mouse = 0,
        .keyboard = 0
    };
}

static void drawUpgradeMenu(void * data) {
    AppState * as = (AppState *)data;

    fillRect(as->renderer, 100, 100, 100, 100);
    SDL_RenderPresent(as->renderer);
}

void updatePlayer(void * data, float dt) {
    int w, h;
    AppState * as = (AppState *)data;
    
    if (!SDL_GetRenderOutputSize(as->renderer, &w, &h)) {
        return;
    }

    Player * player = as->player;
    Enemy * enemies = as->enemies;
    Camera * camera = as->camera;

    const int speed = player->speed;
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

    bool collision_detected = false;

    SDL_FRect player_rect = {
        .x = w/2 - player->w/8,
        .y = h/2 - player->h/8,
        .w = player->w / 4,
        .h = player->h / 4
    };

    for (int i = 0; i < as->enemy_number; i++) {
        Enemy * enemy = &enemies[i];

        SDL_FRect enemy_rect = {
            .x = enemies[i].x - enemies[i].w/2 - camera->x,      
            .y = enemies[i].y - enemies[i].h/2 - camera->y,
            .w = enemies[i].w,    
            .h = enemies[i].h
        };

        if (SDL_HasRectIntersectionFloat(&player_rect, &enemy_rect)) {
            collision_detected = true;
            if(!(player->health - 10 < 0)) {
                player->health -= 10;
            }
            break;
        }
    }

    player->x += dx;
    player->y += dy;

    if(player->progression_to_next_level >= 100) {
        player->progression_to_next_level = 0;
        player->level ++;
    }
}

void drawPlayer(void * data) {
    int w, h;
    AppState * as = (AppState *)data;
    SDL_Renderer * renderer = as->renderer;

    if (!SDL_GetRenderOutputSize(renderer, &w, &h)) {
        return;
    }

    Player * player = as->player;
    Camera * camera = as->camera;

    static float im_w = 0, im_h = 0;
    static SDL_Texture *image_texture[3] = {NULL};

    if (!image_texture[0]) {
        image_texture[0] = SDL_CreateTextureFromSurface(renderer, CreateSurfaceFromMemory(archer_left_png, archer_left_png_len));
        image_texture[1] = SDL_CreateTextureFromSurface(renderer, CreateSurfaceFromMemory(archer_left_down_png, archer_left_down_png_len));
        image_texture[2] = SDL_CreateTextureFromSurface(renderer, CreateSurfaceFromMemory(archer_left_up_png, archer_left_up_png_len));
        SDL_GetTextureSize(image_texture[0], &im_w, &im_h);
    }

    player->w = im_w;
    player->h = im_h;

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
        .x = w/2 - im_w/8,
        .y = h/2 - im_h/8,
        .w = im_w / 4,
        .h = im_h / 4
    };

    SDL_RenderTextureRotated(renderer, current_texture, NULL, &dest_rect, 0, NULL, flip_mode);

    if(as->debug_mode) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 100);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_RenderFillRect(renderer, &dest_rect);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }

    SDL_SetRenderDrawColor(renderer, 22, 22, 22, 255); // Couleur du fond de la barre
    fillRect(renderer, w / 2 - 35, h / 2 - 80, 70, 10);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Couleur de la vie restante
    fillRect(renderer, w / 2 - 35, h / 2 - 80, (float)player->health / 100 * 70, 10);
}