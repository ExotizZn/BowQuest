#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <math.h>

#include "../assets/Sprite-WhiteHand.h"

#include "../include/appstate.h"
#include "../include/player.h"
#include "../include/enemy.h"
#include "../include/projectile.h"
#include "../include/camera.h"
#include "../include/menu.h"
#include "../include/upgrade.h"
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

#define CHECK_SDL(x, msg) do { if (!(x)) { SDL_Log(msg ": %s", SDL_GetError()); return SDL_APP_FAILURE; } } while (0)

#define QUIT_LOG (bool)false

static Mix_Music *music = NULL;

static SDL_Texture *damageTexture = NULL;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    CHECK_SDL(SDL_SetAppMetadata("Bow Quest", "1.0", ""), "Failed to set app metadata");

    AppState *as = SDL_calloc(1, sizeof(AppState));
    CHECK_SDL(as, "Failed to allocate AppState");
    *appstate = as;

    CHECK_SDL(SDL_Init(SDL_INIT_VIDEO), "Failed to init SDL");
    CHECK_SDL(SDL_CreateWindowAndRenderer("Bow Quest", 1024, 720, SDL_WINDOW_RESIZABLE, &as->window, &as->renderer), "Failed to create window/renderer");
    CHECK_SDL(TTF_Init(), "Failed to init TTF");

    /* if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        return SDL_APP_FAILURE;
    }
    SDL_AudioSpec spec = { .freq = 48000, .format = SDL_AUDIO_F32, .channels = 2 };
    CHECK_SDL(Mix_OpenAudio(0, &spec), "Failed to open audio");
    Mix_VolumeMusic(50);

    music = Mix_LoadMUS("./assets/son.mp3");

    Mix_FadeInMusic(music, 0, 2000); */

    initCamera(&as->camera);
    initPlayer(&as->player);
    as->projectiles = SDL_calloc(100, sizeof(Projectile));
    as->enemy_number = 32;
    as->enemies = SDL_calloc(as->enemy_number, sizeof(Enemy));
    as->fonts = SDL_calloc(1, sizeof(Fonts));
    initMouse(&as->mouse);
    CHECK_SDL(as->camera && as->player && as->projectiles && as->enemies && as->fonts && as->mouse, "Failed to allocate sub-structures");

    as->projectile_number = 0;
    as->current_enemy_number = 0;

    as->is_paused = false;
    as->upgrade_menu = false;
    as->debug_mode = false;

    SDL_Surface *image_surface = CreateSurfaceFromMemory(Sprite_WhiteHand_png, Sprite_WhiteHand_png_len);
    CHECK_SDL(image_surface, "Failed to create surface");
    as->texture = SDL_CreateTextureFromSurface(as->renderer, image_surface);
    SDL_DestroySurface(image_surface);
    CHECK_SDL(as->texture, "Failed to create texture");

    as->skills_assets = SDL_calloc(23, sizeof(SDL_Texture));
    CHECK_SDL(as->skills_assets, "Failed to allocate skills_assets");
    loadItemsAssets(as, as->skills_assets);

    for(int i = 0; i < as->enemy_number; i++) {
        as->enemies[i].w = as->texture->w;
        as->enemies[i].h = as->texture->h;
    }

    loadFonts(as->fonts);
    as->page = 1;
    SDL_SetAtomicInt(&as->running, 1);

    as->enemyMutex = SDL_CreateMutex();
    as->enemyThread = SDL_CreateThread(enemyUpdateThread, "EnemyUpdateThread", as);
    CHECK_SDL(as->enemyMutex && as->enemyThread, "Failed to create mutex or thread");

    int w, h;
    SDL_GetRenderOutputSize(as->renderer, &w, &h);

    damageTexture = SDL_CreateTexture(as->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);

    SDL_SetRenderTarget(as->renderer, damageTexture);
    SDL_SetRenderDrawBlendMode(as->renderer, SDL_BLENDMODE_BLEND);

    SDL_SetRenderDrawColor(as->renderer, 0, 0, 0, 0);
    SDL_RenderClear(as->renderer);

    int center_x = w / 2;
    int center_y = h / 2;
    float max_radius = SDL_sqrt(center_x * center_x + center_y * center_y); // Distance to corner
    float vignette_radius = max_radius * 0.4f;
    
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            // Calculate distance from center
            float dx = x - center_x;
            float dy = y - center_y;
            float distance = SDL_sqrt(dx * dx + dy * dy);
            
            // Normalize distance relative to vignette_radius
            float t = (distance - (max_radius - vignette_radius)) / vignette_radius;
            if (t < 0.0f) t = 0.0f; // Transparent inside vignette radius
            if (t > 1.0f) t = 1.0f; // Fully red beyond vignette radius
            
            // Alpha increases toward edges, but only within vignette area
            Uint8 alpha = (Uint8)(255 * t);
            SDL_SetRenderDrawColor(as->renderer, 255, 0, 0, alpha);
            SDL_RenderPoint(as->renderer, x, y);
        }
    }
    
    SDL_SetRenderTarget(as->renderer, NULL);
    SDL_SetHintWithPriority(SDL_HINT_WINDOWS_RAW_KEYBOARD, "1", SDL_HINT_OVERRIDE);
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    AppState *as = appstate;
    Player *player = as->player;

    int w, h;
    SDL_GetRenderOutputSize(as->renderer, &w, &h);
    SDL_GetMouseState(&as->mouse->x, &as->mouse->y);

    switch (event->type) {
    case SDL_EVENT_QUIT:
        return SDL_APP_SUCCESS;
    case SDL_EVENT_WINDOW_FOCUS_LOST:
        if(as->page == 1) as->is_paused = true;
        break;
    case SDL_EVENT_KEY_UP: {
        SDL_Keycode sym = event->key.key;
        if (sym == SDLK_L) return SDL_APP_SUCCESS;
        if (sym == SDLK_ESCAPE) as->is_paused = !as->is_paused;
        if (sym == SDLK_F11) {
            static bool fullscreen = false;
            fullscreen = !fullscreen;
            SDL_SetWindowFullscreen(as->window, fullscreen);
        }
        if (sym == SDLK_Z) KEY_RELEASE(player->zqsd, 0x01); // Z : bit 0
        if (sym == SDLK_Q) KEY_RELEASE(player->zqsd, 0x02); // Q : bit 1
        if (sym == SDLK_S) KEY_RELEASE(player->zqsd, 0x04); // S : bit 2
        if (sym == SDLK_D) KEY_RELEASE(player->zqsd, 0x08); // D : bit 3
        break;
    }
    case SDL_EVENT_KEY_DOWN: {
        SDL_Keycode sym = event->key.key;
        if (sym == SDLK_P) as->debug_mode = !as->debug_mode;
        if (as->is_paused || as->upgrade_menu) break;
        if (sym == SDLK_Z) KEY_PRESS(player->zqsd, 0x01); // Z : bit 0
        if (sym == SDLK_Q) KEY_PRESS(player->zqsd, 0x02); // Q : bit 1
        if (sym == SDLK_S) KEY_PRESS(player->zqsd, 0x04); // S : bit 2
        if (sym == SDLK_D) KEY_PRESS(player->zqsd, 0x08); // D : bit 3
        break;
    }
    case SDL_EVENT_MOUSE_BUTTON_UP:
        if(event->button.button == SDL_BUTTON_LEFT) as->mouse->left_button = false;
        break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if(event->button.button == SDL_BUTTON_LEFT) {
            as->mouse->left_button = true;
            if(!as->is_paused && as->debug_mode) addProjectileDebugMode(as, as->mouse->x, as->mouse->y, w, h);
        }
        break;
    }
    return SDL_APP_CONTINUE;
}

void draw(AppState *as, Uint64 dt_ns) {
    SDL_Renderer * renderer = as->renderer;

    int w, h;
    if (!SDL_GetRenderOutputSize(renderer, &w, &h)) return;

    const float dt = dt_ns / 1e9f;
    SDL_SetRenderDrawColor(renderer, 22, 22, 22, 255);
    SDL_RenderClear(renderer);

    static char level_text[16] = "Lvl. 0";
    static char player_x[16] = "x: 0.00";
    static char player_y[16] = "y: 0.00";
    static char enemy_number_text[32] = "Ennemis: 0";

    switch(as->page) {
    case 0:
        drawMenu(as, w, h);
        break;
    case 1:
        if(as->debug_mode) {
            drawGrid(renderer, as->camera, w, h);
            drawLine(renderer, w / 2, h / 2, as->mouse->x, as->mouse->y, COLOR_RED);
        } else {
            drawBackground(renderer, as->camera, w, h);
        }

        drawEnemies(as);
        drawProjectiles(as);
        drawPlayer(as);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_snprintf(level_text, sizeof(level_text), "Lvl. %d", as->player->level);
        drawText(as, level_text, as->fonts->poppins_semibold_16, w/2, 15, COLOR_WHITE, true);

        if (as->debug_mode) {
            SDL_snprintf(player_x, sizeof(player_x), "x: %.2f", as->player->x);
            SDL_snprintf(player_y, sizeof(player_y), "y: %.2f", as->player->y);
            SDL_snprintf(enemy_number_text, sizeof(enemy_number_text), "Ennemis: %d", as->current_enemy_number);
            drawText(as, debug_string, as->fonts->poppins_medium_12, 10, 5, COLOR_WHITE, false);
            drawText(as, player_x, as->fonts->poppins_medium_12, 10, 20, COLOR_WHITE, false);
            drawText(as, player_y, as->fonts->poppins_medium_12, 10, 35, COLOR_WHITE, false);
            drawText(as, enemy_number_text, as->fonts->poppins_medium_12, 10, 50, COLOR_WHITE, false);
        }

        roundRect(renderer, w/2-200, 30, 400, 25, 5, COLOR_GREEN);
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        fillRect(renderer, w/2+200+2, 30, as->player->progression_to_next_level/100*400 - 402, 26);

        drawText(as, "Bow Quest (build 1.0.0)", as->fonts->poppins_medium_10, 10, h-20, as->debug_mode ? RGBA(150, 150, 150, 255) : RGBA(55, 55, 55, 255), false);

        if(as->upgrade_menu) drawUpgradeMenu(as);

        if(as->is_paused) {
            SDL_SetRenderDrawBlendMode(as->renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
            fillRect(renderer, 0, 0, w, h);
            SDL_SetRenderDrawBlendMode(as->renderer, SDL_BLENDMODE_NONE);

            drawText(as, "Pause", as->fonts->poppins_semibold_16, w/2, 50, RGBA(255, 255, 255, 255), true);

            const char *btn_text[3] = {"Reprendre", "Options", "Quitter"};

            for (int i = 0; i < 3; i++) {
                int y = h / 2 + (i * 100) - 125;
                bool hovered = as->mouse->x > w / 2 - 100 && as->mouse->x < w / 2 + 100 &&
                               as->mouse->y > y && as->mouse->y < y + 50;
                roundRect(renderer, w / 2 - 100, y, 200, 50, 5, hovered ? RGBA(160, 0, 0, 255) : RGBA(255, 0, 0, 255));
                if (hovered && as->mouse->left_button) {
                    if (i == 0) as->is_paused = false;
                    else if (i == 1) SDL_SetWindowSize(as->window, 800, 800);
                    else if (i == 2) as->page = 0;
                }
                drawText(as, btn_text[i], as->fonts->poppins_semibold_16, w / 2, y + 25, COLOR_WHITE, true);
            }
        }
        break;
    }

    static float time = 0.0f;
    const float blink_frequency = 1.0f * 2.0f * M_PI;

    if(as->player->is_hit) {
        time += 0.016f;
        float blink_factor = (SDL_sinf(blink_frequency * time) + 1.0f) / 2.0f; // Oscillates 0 to 1
        Uint8 texture_alpha = (Uint8)(255 * blink_factor);
        SDL_SetTextureAlphaMod(damageTexture, texture_alpha);
        SDL_RenderTexture(renderer, damageTexture, NULL, NULL);
    }

    SDL_RenderPresent(renderer);
}

void update(AppState *as, Uint64 dt_ns) {
    int w, h;
    if (!SDL_GetRenderOutputSize(as->renderer, &w, &h)) {
        return;
    }

    Player *player = as->player;
    float dt = dt_ns / 1e9f;

    updatePlayer(as, dt);
    updateProjectiles(as, w, h, dt);

    as->camera->x = player->x - w / 2;
    as->camera->y = player->y - h / 2;

    float now = SDL_GetTicksNS() / 1e9f;
    static float last_shot_time = 0;
    if (now - last_shot_time > 1.5f) {
        Enemy *target = findClosestEnemy(as);
        if (target) {
            addProjectile(as, target->x, target->y);
            last_shot_time = now;
        }
    }
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    AppState *as = appstate;
    static Uint64 last = 0;
    Uint64 now = SDL_GetTicksNS();
    Uint64 dt_ns = now - last;

    if (!as->is_paused && !as->upgrade_menu) update(as, dt_ns);
    draw(as, dt_ns);

    static Uint64 frame_count = 0;
    static Uint64 fps_timer = 0;
    fps_timer += dt_ns;
    frame_count++;
    if (fps_timer >= 1e9) {
        SDL_snprintf(debug_string, sizeof(debug_string), "%" SDL_PRIu64 " fps", frame_count);
        frame_count = 0;
        fps_timer -= 1e9;
    }

    last = now;
    Uint64 elapsed = SDL_GetTicksNS() - now;
    if (elapsed < 16666666) {
        SDL_DelayNS(16666666 - elapsed);
    }

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    AppState *as = (AppState *)appstate;
    if (!as) {
        SDL_Log("AppState is null, nothing to clean up");
        return;
    }

    if (QUIT_LOG) SDL_Log("Shutting down...");
    SDL_SetAtomicInt(&as->running, 0);
    if (as->enemyThread) {
        if (QUIT_LOG) SDL_Log("Waiting for enemy thread...");
        SDL_WaitThread(as->enemyThread, NULL);
        as->enemyThread = NULL;
    }
    if (as->enemyMutex) {
        if (QUIT_LOG) SDL_Log("Destroying enemy mutex...");
        SDL_DestroyMutex(as->enemyMutex);
        as->enemyMutex = NULL;
    }

    if (as->player) {
        if (as->player->player_stats) {
            if (QUIT_LOG) SDL_Log("Freeing player stats...");
            SDL_free(as->player->player_stats);
            as->player->player_stats = NULL;
        }
        if (QUIT_LOG) SDL_Log("Freeing player...");
        SDL_free(as->player);
        as->player = NULL;
    }
    if (as->projectiles) {
        if (QUIT_LOG) SDL_Log("Freeing projectiles...");
        SDL_free(as->projectiles);
        as->projectiles = NULL;
    }
    if (as->enemies) {
        if (QUIT_LOG) SDL_Log("Freeing enemies...");
        SDL_free(as->enemies);
        as->enemies = NULL;
    }

    if (as->fonts) {
        if (as->fonts->poppins_semibold_16) {
            if (QUIT_LOG) SDL_Log("Closing poppins_semibold_16...");
            TTF_CloseFont(as->fonts->poppins_semibold_16);
            as->fonts->poppins_semibold_16 = NULL;
        }
        if (as->fonts->poppins_medium_14) {
            if (QUIT_LOG)  SDL_Log("Closing poppins_medium_14...");
            TTF_CloseFont(as->fonts->poppins_medium_14);
            as->fonts->poppins_medium_14 = NULL;
        }
        if (as->fonts->poppins_regular_12) {
            if (QUIT_LOG) SDL_Log("Closing poppins_regular_12...");
            TTF_CloseFont(as->fonts->poppins_regular_12);
            as->fonts->poppins_regular_12 = NULL;
        }
        if (as->fonts->poppins_medium_12) {
            if (QUIT_LOG) SDL_Log("Closing poppins_medium_12...");
            TTF_CloseFont(as->fonts->poppins_medium_12);
            as->fonts->poppins_medium_12 = NULL;
        }
        if (as->fonts->poppins_medium_10) {
            if (QUIT_LOG) SDL_Log("Closing poppins_medium_10...");
            TTF_CloseFont(as->fonts->poppins_medium_10);
            as->fonts->poppins_medium_10 = NULL;
        }
        if (QUIT_LOG) SDL_Log("Freeing fonts...");
        SDL_free(as->fonts);
        as->fonts = NULL;
    }

    if (as->mouse) {
        if (QUIT_LOG) SDL_Log("Freeing mouse...");
        SDL_free(as->mouse);
        as->mouse = NULL;
    }
    if (as->camera) {
        if (QUIT_LOG) SDL_Log("Freeing camera...");
        SDL_free(as->camera);
        as->camera = NULL;
    }

    if (as->texture) {
        if (QUIT_LOG) SDL_Log("Destroying enemy texture...");
        SDL_DestroyTexture(as->texture);
        as->texture = NULL;
    }
    if (as->skills_assets) {
        for (int i = 0; i < 23; i++) {
            if (as->skills_assets[i]) {
                if (QUIT_LOG) SDL_Log("Destroying skills_assets[%d]...", i);
                SDL_DestroyTexture(as->skills_assets[i]);
                as->skills_assets[i] = NULL;
            }
        }
        if (QUIT_LOG) SDL_Log("Freeing skills_assets array...");
        SDL_free(as->skills_assets);
        as->skills_assets = NULL;
    }

    if (QUIT_LOG) SDL_Log("Cleaning up player textures...");
    cleanupPlayerTextures();
    if (QUIT_LOG) SDL_Log("Cleaning up projectile texture...");
    cleanupProjectileTexture();
    if (QUIT_LOG) SDL_Log("Cleaning up upgrade menu cursors...");
    cleanupUpgradeMenuCursors();

    if (as->renderer) {
        if (QUIT_LOG) SDL_Log("Destroying renderer...");
        SDL_DestroyRenderer(as->renderer);
        as->renderer = NULL;
    }
    if (as->window) {
        if (QUIT_LOG) SDL_Log("Destroying window...");
        SDL_DestroyWindow(as->window);
        as->window = NULL;
    }

    if (QUIT_LOG) SDL_Log("Shutting down TTF...");
    TTF_Quit();
    if (QUIT_LOG) SDL_Log("Shutting down Mixer...");
    Mix_CloseAudio();
    if (QUIT_LOG) SDL_Log("Shutting down SDL...");
    SDL_Quit();

    if (QUIT_LOG) SDL_Log("Freeing AppState...");
    SDL_free(as);

    if (QUIT_LOG) SDL_Log("Done.");
}