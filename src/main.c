#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL_opengl.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>

#include "../assets/Sprite-WhiteHand.h"

#include "../include/appstate.h"
#include "../include/player.h"
#include "../include/enemy.h"
#include "../include/projectile.h"
#include "../include/camera.h"
#include "../include/menu.h"
#include "../include/utils.h"
#include "../include/fonts.h"

static char debug_string[8];

#define KEY_PRESS(state, key) ((state) |= (key))
#define KEY_RELEASE(state, key) ((state) &= ~(key))

#define COLOR_WHITE (SDL_Color){255, 255, 255, 255}
#define COLOR_RED   (SDL_Color){255, 0, 0, 255}
#define COLOR_GREEN (SDL_Color){0, 255, 0, 255}
#define COLOR_BLUE  (SDL_Color){0, 0, 255, 255}
#define COLOR_TRANSPARENT (SDL_Color){0, 0, 0, 0}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    if (!SDL_SetAppMetadata("Archero Remake", "1.0", "")) {
        return SDL_APP_FAILURE;
    }

    AppState *as = SDL_calloc(1, sizeof(AppState));
    if (!as) {
        return SDL_APP_FAILURE;
    } else {
        *appstate = as;
    }

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("Archero C", 1024, 720, SDL_WINDOW_RESIZABLE, &as->window, &as->renderer)) {
        return SDL_APP_FAILURE;
    }

    if(!TTF_Init()) {
        return SDL_APP_FAILURE;
    }

    // Initialisation de la camera
    as->camera = SDL_calloc(1, sizeof(Camera));

    // Initialisation du joueur
    as->player = SDL_calloc(1, sizeof(Player));
    initPlayers(as->player);

    // Initialisation des projectiles
    as->projectiles = SDL_calloc(100, sizeof(Projectile));
    as->projectile_number = 0;

    // Initialisation des ennemies 
    as->enemy_number = 32;
    as->enemies = SDL_calloc(as->enemy_number, sizeof(Enemy));
    initEnemies(as->enemies, as->enemy_number);

    SDL_Surface* image_surface = CreateSurfaceFromMemory(Sprite_WhiteHand_png, Sprite_WhiteHand_png_len);
    SDL_Texture *image_texture = SDL_CreateTextureFromSurface(as->renderer, image_surface);
    SDL_DestroySurface(image_surface);

    as->texture = image_texture;

    for(int i = 0; i < as->enemy_number; i++) {
        as->enemies[i].w = image_texture->w;
        as->enemies[i].h = image_texture->h;
    }

    // Initialisation des polices d'écriture
    as->fonts = SDL_calloc(1, sizeof(Fonts));
    loadFonts(as->fonts);

    as->page = 1;
    as->is_paused = false;
    as->debug_mode = false;

    SDL_SetAtomicInt(&as->running, 1);

    as->enemyMutex = SDL_CreateMutex();
    if (!as->enemyMutex) {
        return SDL_APP_FAILURE;
    }
    as->enemyThread = SDL_CreateThread(enemyUpdateThread, "EnemyUpdateThread", as);
    if (!as->enemyThread) {
        SDL_DestroyMutex(as->enemyMutex);
        return SDL_APP_FAILURE;
    }

    SDL_SetHintWithPriority(SDL_HINT_WINDOWS_RAW_KEYBOARD, "1", SDL_HINT_OVERRIDE);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    AppState *as = appstate;
    Player *player = as->player;

    static bool fullscreen = false;

    int w, h;
    float mouse_x, mouse_y;

    SDL_GetRenderOutputSize(as->renderer, &w, &h);
    SDL_GetMouseState(&mouse_x, &mouse_y);

    switch (event->type) {
        case SDL_EVENT_QUIT:
            return SDL_APP_SUCCESS;
        case SDL_EVENT_WINDOW_FOCUS_LOST:
            if(as->page == 1) {
                as->is_paused = true;
            }
            break;
        case SDL_EVENT_KEY_UP: {
            SDL_Keycode sym = event->key.key;
            //SDL_KeyboardID id = event->key.which;
            if (sym == SDLK_L && as->page == 1) return SDL_APP_SUCCESS;
            if (sym == SDLK_ESCAPE) as->is_paused = !as->is_paused;
            if (sym == SDLK_Z) KEY_RELEASE(player->zqsd, 0x01); // Z : bit 0
            if (sym == SDLK_Q) KEY_RELEASE(player->zqsd, 0x02); // Q : bit 1
            if (sym == SDLK_S) KEY_RELEASE(player->zqsd, 0x04); // S : bit 2
            if (sym == SDLK_D) KEY_RELEASE(player->zqsd, 0x08); // D : bit 3
            if (sym == SDLK_F11) {
                fullscreen = !fullscreen;
                if(fullscreen) {
                    SDL_SetWindowSize(as->window, 1920, 1080);
                }
                SDL_SetWindowFullscreen(as->window, fullscreen);
                SDL_SetWindowSize(as->window, 1024, 720);
                SDL_SetWindowPosition(as->window, 0, 0);
            };
            break;
        }
        case SDL_EVENT_KEY_DOWN: {
            SDL_Keycode sym = event->key.key;
            //SDL_KeyboardID id = event->key.which;
            if (sym == SDLK_Z) KEY_PRESS(player->zqsd, 0x01); // Z : bit 0
            if (sym == SDLK_Q) KEY_PRESS(player->zqsd, 0x02); // Q : bit 1
            if (sym == SDLK_S) KEY_PRESS(player->zqsd, 0x04); // S : bit 2
            if (sym == SDLK_D) KEY_PRESS(player->zqsd, 0x08); // D : bit 3
            if (sym == SDLK_P) as->debug_mode = !as->debug_mode;
            break;
        }
        case SDL_EVENT_MOUSE_BUTTON_DOWN: {
            if(event->button.button == SDL_BUTTON_LEFT) {
                if(!as->is_paused) {
                    float angle = SDL_atan2(mouse_y - h/2, mouse_x - w/2);
                    float cos_angle = SDL_cos(angle);
                    float sin_angle = SDL_sin(angle);

                    initProjectile(
                        &as->projectiles[as->projectile_number],
                        player->x,
                        player->y,
                        cos_angle,
                        sin_angle,
                        angle
                    );

                    as->projectile_number++;
                } else {
                    for(int i = 0; i < 3; i++) {
                        if(mouse_x > w/2-100 && mouse_x < w/2+100) {
                            if(mouse_y > h/2 - (50*5)/2 + (i*100) && mouse_y < h/2 - (50*5)/2 + (i*100) + 50) {
                                switch(i) {
                                    case 0:
                                        as->is_paused = false;
                                        break;
                                    case 1:
                                        SDL_SetWindowSize(as->window, 800, 800);
                                        break;
                                    case 2:
                                        as->page = 0;
                                        break;
                                    default:
                                        break;
                                }
                            }
                        }
                    }
                }
            }
            break;
        }
    }
    return SDL_APP_CONTINUE;
}

void draw(AppState *as, Uint64 dt_ns) { 
    SDL_Renderer * renderer = as->renderer;
    Player * player = as->player;
    Camera * camera = as->camera;
    Fonts * fonts = as->fonts;
    Projectile * projectiles = as->projectiles;

    int w, h;
    if (!SDL_GetRenderOutputSize(renderer, &w, &h)) {
        return;
    }

    float mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);

    SDL_SetRenderDrawColor(renderer, 22, 22, 22, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderClipRect(renderer, 0);

    const float dt = dt_ns / 1e9f;

    switch(as->page) {
        case 0:
            drawMenu(as, w, h);
            break;
        case 1:
            if(as->debug_mode) {
                drawGrid(renderer, camera, w, h);
                drawLine(renderer, w/2, h/2, mouse_x, mouse_y, COLOR_RED);
            } else {
                drawBackground(renderer, camera, w, h);
            }

            drawEnemies(as);
            drawProjectiles(as);
            drawPlayer(as);

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

            char level_text[10] = "Lvl. 0";
            char player_x[10];
            char player_y[10];

            SDL_snprintf(level_text, sizeof(level_text), "Lvl. %d", player->level);
            drawText(as, level_text, fonts->poppins_semibold_16, w/2, 15, COLOR_WHITE, true);

            SDL_snprintf(player_x, sizeof(player_x), "x: %.2f", player->x);
            SDL_snprintf(player_y, sizeof(player_y), "y: %.2f", player->y);

            drawText(as, debug_string, fonts->poppins_medium_12, 10, 5, COLOR_WHITE, false);
            drawText(as, player_x, fonts->poppins_medium_12, 10, 20, COLOR_WHITE, false);
            drawText(as, player_y, fonts->poppins_medium_12, 10, 35, COLOR_WHITE, false);

            roundRect(renderer, w/2-200, 30, 400, 25, 5, COLOR_GREEN);
            SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
            fillRect(renderer, w/2+200+2, 30, player->progression_to_next_level/100*400 - 402, 26);

            if(as->debug_mode) {
                drawText(as, "Archero-C (build 1.0.0)", fonts->poppins_medium_10, 10, h-20, RGBA(150, 150, 150, 255), false);
            } else {
                drawText(as, "Archero-C (build 1.0.0)", fonts->poppins_medium_10, 10, h-20, RGBA(55, 55, 55, 255), false);
            }

            if(as->is_paused) {
                SDL_SetRenderDrawBlendMode(as->renderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
                fillRect(renderer, 0, 0, w, h);
                SDL_SetRenderDrawBlendMode(as->renderer, SDL_BLENDMODE_NONE);

                drawText(as, "Pause", fonts->poppins_semibold_16, w/2, 50, RGBA(255, 255, 255, 255), true);

                const char * btn_text[3] = {"Reprendre", "Options", "Quitter"};

                for(int i = 0; i < 3; i++) {
                    roundRect(renderer, w/2-100, h/2 - (50*5)/2 + (i*100), 200, 50, 5, RGBA(255, 0, 0, 255));
                    drawText(as, btn_text[i], fonts->poppins_semibold_16, w/2, h/2 - (50*5)/2 + (i*100) + 50/2, RGBA(255, 255, 255, 255), true);
                }
            }
            break;
        default:
            break;
    }

    SDL_RenderPresent(renderer);
}

void update(AppState *as, Uint64 dt_ns) {
    int w, h;
    if (!SDL_GetRenderOutputSize(as->renderer, &w, &h)) {
        return;
    }

    Player * player = as->player;
    float dt = dt_ns / 1e9f;

    updatePlayer(as, dt);
    updateProjectiles(as, w, h, dt);

    as->camera->x = player->x - w/2;
    as->camera->y = player->y - h/2;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    AppState *as = appstate;
    
    static Uint64 accu = 0;
    static Uint64 last = 0;
    static Uint64 past = 0;
    Uint64 now = SDL_GetTicksNS();
    Uint64 dt_ns = now - past;

    if(!as->is_paused) {
        update(as, dt_ns);
    }

    draw(as, dt_ns);

    if (now - last > 999999999) {
        last = now;
        SDL_snprintf(debug_string, sizeof(debug_string), "%" SDL_PRIu64 " fps", accu);
        accu = 0;
    }

    past = now;
    accu ++;
    Uint64 elapsed = SDL_GetTicksNS() - now;
    if (elapsed < 999999) {
        SDL_DelayNS(999999 - elapsed);
    }

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    AppState *as = (AppState *)appstate;
    SDL_SetAtomicInt(&as->running, 0);

    SDL_WaitThread(as->enemyThread, NULL);
    SDL_DestroyMutex(as->enemyMutex);

    SDL_free(as);
}