#include <SDL3/SDL.h>
#include <math.h>

#include "../include/appstate.h"
#include "../include/player.h"
#include "../include/upgrade.h"
#include "../include/utils.h"

#include "../assets/archer/archer_left.h"
#include "../assets/archer/archer_left_down.h"
#include "../assets/archer/archer_left_up.h"
#include "../assets/coin.h"
#include "../assets/game_over.h"

#define DEFAULT_PROJECTILE_DELAY 100.0f
#define DEFAULT_PROJECTILE_SPEED 100.0f
#define DEFAULT_PROJECTILE_DAMAGE 10.0f
#define DEFAULT_CRIT_CHANCE 1.0f
#define DEFAULT_SPEED 200.0f
#define DEFAULT_HEALTH 100.0f

void initPlayer(Player **player) {
    *player = SDL_calloc(1, sizeof(Player));

    Player_Stats *stats = SDL_calloc(1, sizeof(Player_Stats));
    if (!stats) {
        SDL_Log("Failed to allocate Player_Stats");
        return;
    }

    *stats = (Player_Stats){
        .projectile_delay = DEFAULT_PROJECTILE_DELAY,
        .projectile_speed = DEFAULT_PROJECTILE_SPEED,
        .projectile_damage = DEFAULT_PROJECTILE_DAMAGE,
        .crit_chance = DEFAULT_CRIT_CHANCE,
        .armor = 10,
        .regen = 0
    };

    (*player)->x = 1000;
    (*player)->y = 1000;
    (*player)->w = 0;
    (*player)->h = 0;
    (*player)->level = 0;
    (*player)->speed = 200;
    (*player)->progression_to_next_level = 0;
    (*player)->health = 100;
    (*player)->coins = 0;
    (*player)->is_hit = false;
    (*player)->zqsd = 0;
    (*player)->mouse = 0;
    (*player)->keyboard = 0;
    (*player)->player_stats = stats;
}

static SDL_Cursor *cursor_pointer = NULL, *cursor_default = NULL;

void cleanupUpgradeMenuCursors(void) {
    if (cursor_pointer) {
        SDL_DestroyCursor(cursor_pointer);
        cursor_pointer = NULL;
    }
    if (cursor_default) {
        SDL_DestroyCursor(cursor_default);
        cursor_default = NULL;
    }
}

void drawUpgradeMenu(void *data) {
    AppState *as = (AppState *)data;
    SDL_Renderer *renderer = as->renderer;

    if (!as || !renderer || !as->fonts) return;

    int w, h;
    if (!SDL_GetRenderOutputSize(renderer, &w, &h)) return;

    float mouse_x = (float)as->mouse->x, mouse_y = (float)as->mouse->y;

    if (!cursor_pointer) {
        cursor_pointer = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER);
        cursor_default = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
    }
    
    static bool is_active = false;
    SDL_SetCursor(is_active ? cursor_pointer : cursor_default);
    is_active = false;
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
    fillRect(renderer, 0, 0, w, h);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    drawText(as, "Améliorations", as->fonts->poppins_semibold_16, w / 2, h / 2 - 200, RGBA(255, 255, 255, 255), true);

    static char *skills_text[] = {"Chances accrues", "Boost attaque", "Flèches critiques"};
    static bool shuffled = false;
    static int rand[3] = {0};

    if(!shuffled) {
        for(int i = 0; i < 3; i++) rand[i] = SDL_rand(8);
        shuffled = true;
    }

    const int base_x = w / 2 - 345, y_top = h / 2 - 155;
    for(int i = 0; i < 3; i++) {
        int x = base_x + i * 240;
        bool hovered = mouse_x > x && mouse_x < x + 230 && mouse_y > y_top && mouse_y < y_top + 310;

        roundRect(as->renderer, x, y_top, 230, 310, 10, hovered ? RGBA(0, 255, 0, 255) : RGBA(255, 255, 255, 255));
        roundRect(as->renderer, x + 5, y_top + 5, 220, 300, 10, hovered ? RGBA(43, 63, 95, 255) : RGBA(33, 53, 85, 255));

        if (hovered) {
            is_active = true;
            if (as->mouse->left_button) {
                shuffled = false;
                as->upgrade_menu = false;
                SDL_SetCursor(cursor_default);
            }
        }

        drawItem(as, x + 65, y_top + 25, 100, 100, rand[i]);
        drawText(as, skills_text[i], as->fonts->poppins_medium_14, x + 115, y_top + 145, RGBA(255, 255, 255, 255), true);
        drawTextWrapped(as, "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vestibulum tempor nunc erat. Pellentesque ex enim, dictum at metus sed, finibus semper ante. In volutpat luctus commodo.",
            as->fonts->poppins_regular_12, x + 15, y_top + 165, RGBA(255, 255, 255, 255), 220, false);
    }
}

void drawGameOver(void *data, float dt) {
    AppState *as = (AppState *)data;
    SDL_Renderer *renderer = as->renderer;

    if (!as || !renderer || !as->fonts) return;

    int w, h;
    if (!SDL_GetRenderOutputSize(as->renderer, &w, &h)) return;

    static int alpha = 0;
    static SDL_Texture *game_over = NULL;

    SDL_SetRenderDrawBlendMode(as->renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(as->renderer, 0, 0, 0, alpha);
    fillRect(renderer, 0, 0, w, h);
    SDL_SetRenderDrawBlendMode(as->renderer, SDL_BLENDMODE_NONE);


    if(!game_over) {
        SDL_Surface *game_over_surface = CreateSurfaceFromMemory(game_over_png, game_over_png_len);
        game_over = SDL_CreateTextureFromSurface(as->renderer, game_over_surface);
        SDL_DestroySurface(game_over_surface);
    }

    float game_over_w = 0, game_over_h = 0;
    SDL_GetTextureSize(game_over, &game_over_w, &game_over_h);

    static int game_over_y = 0;

    SDL_FRect game_over_dest= {
        .x = w / 2 - game_over_w / 4,
        .y = h / 2 - game_over_h / 4 - game_over_y,
        .w = game_over_w / 2,
        .h = game_over_h / 2
    };

    SDL_SetTextureAlphaMod(game_over, alpha);
    SDL_RenderTexture(renderer, game_over, NULL, &game_over_dest);

    if(alpha < 255) {
        alpha++;
    } else {
        if(game_over_y < 150) {
            game_over_y++;
        } else {
            const char *btn_text[2] = {"Rejouer", "Quitter"};
            for (int i = 0; i < 2; i++) {
                int y = h / 2 + (i * 100) - 50;
                bool hovered = as->mouse->x > w / 2 - 100 && as->mouse->x < w / 2 + 100 &&
                               as->mouse->y > y && as->mouse->y < y + 50;
                roundRect(renderer, w / 2 - 100, y, 200, 50, 5, hovered ? RGBA(160, 0, 0, 255) : RGBA(255, 0, 0, 255));
                if (hovered && as->mouse->left_button) {
                    if (i == 0) as->is_paused = false;
                    else if (i == 1) as->page = 0;
                }
                drawText(as, btn_text[i], as->fonts->poppins_semibold_16, w / 2, y + 25, RGBA(255, 255, 255, 255), true);
            }
        }
    }
}

void updatePlayer(void *data, float dt) {
    AppState *as = (AppState *)data;

    int w, h;
    if (!SDL_GetRenderOutputSize(as->renderer, &w, &h)) return;

    if(as->game_over) return;

    Player *player = as->player;
    Enemy *enemies = as->enemies;
    Camera *camera = as->camera;

    const int player_speed = player->speed;
    float dx = 0, dy = 0;

    // Calcule les directions en fonction des touches enfoncées
    if (player->zqsd & 0x01) dy -= player_speed * dt; // Z (haut)
    if (player->zqsd & 0x02) dx -= player_speed * dt; // Q (gauche)
    if (player->zqsd & 0x04) dy += player_speed * dt; // S (bas)
    if (player->zqsd & 0x08) dx += player_speed * dt; // D (droite)

    // Calcule la magnitude uniquement si un déplacement est requis
    float magnitude = SDL_sqrtf(dx * dx + dy * dy);
    if (magnitude > 0) {
        dx = (dx / magnitude) * player_speed * dt; // Normalise et applique la vitesse
        dy = (dy / magnitude) * player_speed * dt; // Normalise et applique la vitesse
    }

    SDL_FRect player_rect = {
        .x = w / 2 - player->w / 2,
        .y = h / 2 - player->h / 2,
        .w = player->w,
        .h = player->h
    };

    SDL_LockMutex(as->enemyMutex);
    for (int i = 0; i < as->enemy_number; i++) {
        Enemy *enemy = &enemies[i];

        if(!enemy->active) continue;

        SDL_FRect enemy_rect = {
            .x = enemies[i].x - enemies[i].w / 2 - camera->x,
            .y = enemies[i].y - enemies[i].h / 2 - camera->y,
            .w = enemies[i].w,
            .h = enemies[i].h
        };

        if (SDL_HasRectIntersectionFloat(&player_rect, &enemy_rect)) {
            player->health = SDL_max(0, player->health - 50 * dt);
            player->is_hit = true;
            break;
        }

        player->is_hit = false;
    }
    SDL_UnlockMutex(as->enemyMutex);

    player->x += dx;
    player->y += dy;

    if(player->progression_to_next_level >= 100) {
        as->upgrade_menu = true;
        player->progression_to_next_level = 0;
        player->level++;
    }

    if(player->health == 0) {
        as->game_over = true;
    }
}

static SDL_Texture *textures[3] = {NULL};
static SDL_Texture *coin = NULL;
static float im_w = 0, im_h = 0;
static bool textures_loaded = false;

void cleanupPlayerTextures(void) {
    for (int i = 0; i < 3; i++) {
        if (textures[i]) {
            SDL_DestroyTexture(textures[i]);
            textures[i] = NULL;
        }
    }
    textures_loaded = false;
}

void drawPlayer(void *data) {
    AppState *as = (AppState *)data;
    Player *player = as->player;

    if (!as || !as->renderer || !as->player || !as->camera) return;

    int w, h;
    if (!SDL_GetRenderOutputSize(as->renderer, &w, &h)) return;

    if (!textures_loaded) {
        SDL_Surface *surfaces[3] = {
            CreateSurfaceFromMemory(archer_left_png, archer_left_png_len),
            CreateSurfaceFromMemory(archer_left_down_png, archer_left_down_png_len),
            CreateSurfaceFromMemory(archer_left_up_png, archer_left_up_png_len)
        };
        for (int i = 0; i < 3; i++) {
            if (surfaces[i]) {
                textures[i] = SDL_CreateTextureFromSurface(as->renderer, surfaces[i]);
                SDL_DestroySurface(surfaces[i]);
            }
            if (!textures[i]) SDL_Log("Failed to load texture %d: %s", i, SDL_GetError());
        }

        SDL_Surface *coin_surface = CreateSurfaceFromMemory(coin_png, coin_png_len);
        coin = SDL_CreateTextureFromSurface(as->renderer, coin_surface);

        textures_loaded = true;
    }

    SDL_Texture *current_texture = textures[0];
    SDL_FlipMode flip_mode = (player->zqsd & 0x08) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

    if (player->zqsd & 0x01) current_texture = textures[2];      // Up
    else if (player->zqsd & 0x04) current_texture = textures[1]; // Down

    SDL_GetTextureSize(current_texture, &im_w, &im_h);
    player->w = im_w / 4;
    player->h = im_h / 4;

    SDL_FRect dest_rect = {
        .x = w / 2 - player->w / 2,
        .y = h / 2 - player->h / 2,
        .w = player->w,
        .h = player->h
    };

    if (current_texture) SDL_RenderTextureRotated(as->renderer, current_texture, NULL, &dest_rect, 0, NULL, flip_mode);

    if (as->debug_mode) {
        SDL_SetRenderDrawColor(as->renderer, 0, 0, 255, 100);
        SDL_SetRenderDrawBlendMode(as->renderer, SDL_BLENDMODE_BLEND);
        SDL_RenderFillRect(as->renderer, &dest_rect);
        SDL_SetRenderDrawBlendMode(as->renderer, SDL_BLENDMODE_NONE);
    }

    SDL_SetRenderDrawColor(as->renderer, 22, 22, 22, 255);
    fillRect(as->renderer, w / 2 - 35, h / 2 - 80, 70, 10);
    SDL_SetRenderDrawColor(as->renderer, 216, 64, 64, 255);
    fillRect(as->renderer, w / 2 - 35, h / 2 - 80, player->health * 0.7f, 10);

    if(player->player_stats->armor > 0) {
        SDL_SetRenderDrawColor(as->renderer, 54, 16, 181, 255);
        fillRect(as->renderer, w / 2 + 35 - player->player_stats->armor * 0.7f, h / 2 - 80, player->player_stats->armor * 0.7f, 10);
    }

    SDL_FRect coin_dest = {
        .x = w - 150,
        .y = 21,
        .w = 40,
        .h = 40
    };

    SDL_SetRenderDrawBlendMode(as->renderer, SDL_BLENDMODE_BLEND);
    roundRect(as->renderer, w-165, 15, 140, 52, 5, RGBA(0, 0, 0, 100));
    SDL_SetRenderDrawBlendMode(as->renderer, SDL_BLENDMODE_NONE);

    SDL_RenderTexture(as->renderer, coin, NULL, &coin_dest);
    static char coins[10] = "0";
    SDL_snprintf(coins, sizeof(coins), "%d", as->player->coins);
    drawText(as, coins, as->fonts->poppins_semibold_16, w-100, 30, RGBA(255, 255, 255, 255), false);
}