#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>

#include <stdbool.h>

#include "../assets/Sprite-WhiteHand.h"

#include "../include/player.h"
#include "../include/enemy.h"
#include "../include/projectile.h"
#include "../include/camera.h"
#include "../include/utils.h"

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    Camera *camera;
    Player *player;
    Projectile * projectiles;
    Enemy * enemies;
    SDL_Thread *enemyThread;
    SDL_Mutex *enemyMutex;
    SDL_Mutex *dt_Mutex;
    Uint64 dt_ns;
    int running;
    SDL_Texture * texture;
    int projectile_number;
} AppState;

static char debug_string[32];
static bool debug_mode = false;

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
    if (!SDL_CreateWindowAndRenderer("Archero C", 1024, 768, 0, &as->window, &as->renderer)) {
        return SDL_APP_FAILURE;
    }

    as->camera = SDL_calloc(1, sizeof(Camera));
    as->player = SDL_calloc(1, sizeof(Player));
    as->projectiles = SDL_calloc(1000, sizeof(Projectile));
    as->enemies = SDL_calloc(32, sizeof(Enemy));

    SDL_Surface* image_surface = CreateSurfaceFromMemory(Sprite_WhiteHand_png, Sprite_WhiteHand_png_len);
    SDL_Texture *image_texture = SDL_CreateTextureFromSurface(as->renderer, image_surface);
    SDL_DestroySurface(image_surface);

    as->texture = image_texture;
    as->projectile_number = 0;

    as->enemyMutex = SDL_CreateMutex();
    if (!as->enemyMutex) {
        return SDL_APP_FAILURE;
    }

    as->running = 1;

    as->enemyThread = SDL_CreateThread(enemyUpdateThread, "EnemyUpdateThread", as);
    if (!as->enemyThread) {
        SDL_DestroyMutex(as->enemyMutex);
        return SDL_APP_FAILURE;
    }

    as->dt_Mutex = SDL_CreateMutex();
    if (!as->dt_Mutex) {
        SDL_DestroyMutex(as->enemyMutex);
        return SDL_APP_FAILURE;
    }

    for(int i = 0; i < 32; i++) {
        as->enemies[i].x = SDL_rand(300);
        as->enemies[i].y = SDL_rand(300);
        as->enemies[i].w = 100;
        as->enemies[i].h = 100;
    }

    initPlayers(as->player);
    debug_string[0] = 0;

    SDL_SetHintWithPriority(SDL_HINT_WINDOWS_RAW_KEYBOARD, "1", SDL_HINT_OVERRIDE);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    AppState *as = appstate;
    Player *player = as->player;

    switch (event->type) {
        case SDL_EVENT_QUIT:
            return SDL_APP_SUCCESS;
        case SDL_EVENT_KEY_UP: {
            SDL_Keycode sym = event->key.key;
            //SDL_KeyboardID id = event->key.which;
            if (sym == SDLK_ESCAPE) return SDL_APP_SUCCESS;
            if (sym == SDLK_Z) player->zqsd &= ~0x01; // Z : bit 0
            if (sym == SDLK_Q) player->zqsd &= ~0x02; // Q : bit 1
            if (sym == SDLK_S) player->zqsd &= ~0x04; // S : bit 2
            if (sym == SDLK_D) player->zqsd &= ~0x08; // D : bit 3
            break;
        }
        case SDL_EVENT_KEY_DOWN: {
            SDL_Keycode sym = event->key.key;
            //SDL_KeyboardID id = event->key.which;
            if (sym == SDLK_Z) player->zqsd |= 0x01; // Z : bit 0
            if (sym == SDLK_Q) player->zqsd |= 0x02; // Q : bit 1
            if (sym == SDLK_S) player->zqsd |= 0x04; // S : bit 2
            if (sym == SDLK_D) player->zqsd |= 0x08; // D : bit 3

            if (sym == SDLK_P) debug_mode = !debug_mode;
            break;
        }
        case SDL_EVENT_MOUSE_BUTTON_DOWN: {
            if(event->button.button == SDL_BUTTON_LEFT) {
                int w, h;
                float mouse_x, mouse_y;

                SDL_GetRenderOutputSize(as->renderer, &w, &h);
                SDL_GetMouseState(&mouse_x, &mouse_y);

                float angle = SDL_atan2(mouse_y - h/2, mouse_x - w/2);

                as->projectiles[as->projectile_number].angle = angle;

                as->projectiles[as->projectile_number].x = w/2-25;
                as->projectiles[as->projectile_number].y = h/2-25;

                as->projectiles[as->projectile_number].dx = SDL_cos(angle);
                as->projectiles[as->projectile_number].dy = SDL_sin(angle);

                as->projectile_number++;
            }
            break;
        }
    }
    return SDL_APP_CONTINUE;
}

void draw(AppState *as) { 
    SDL_Renderer * renderer = as->renderer;
    Player * player = as->player;
    Camera * camera = as->camera;
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

    if(debug_mode) {
        drawGrid(renderer, camera, w, h);
    } else {
        drawBackground(renderer, camera, w, h);
    }

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderLine(renderer, w/2, h/2, mouse_x, mouse_y);

    float im_w, im_h;

    SDL_GetTextureSize(as->texture, &im_w, &im_h);

    SDL_LockMutex(as->enemyMutex);
    for(int i = 0; i < 32; i++) {
        SDL_FRect dest_rect;
        dest_rect.x = as->enemies[i].x-im_w/2 - camera->x;             
        dest_rect.y = as->enemies[i].y-im_h/2 - camera->y;             
        dest_rect.w = im_w;         
        dest_rect.h = im_h;

        SDL_RenderTexture(renderer, as->texture, NULL, &dest_rect);
    }
    SDL_UnlockMutex(as->enemyMutex);

    drawProjectiles(renderer, camera, as->projectiles, as->projectile_number);
    drawPlayer(renderer, player, camera, w, h);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    SDL_RenderDebugText(renderer, 10, 10, debug_string);

    static char player_x[32];
    static char player_y[32];
    static char level_text[32] = "Lvl. 0";

    SDL_snprintf(player_x, sizeof(player_x), "x: %.2f", player->x);
    SDL_snprintf(player_y, sizeof(player_y), "y: %.2f", player->y);
    SDL_snprintf(level_text, sizeof(level_text), "Lvl. %d", player->level);

    SDL_RenderDebugText(renderer, w/2-20, 10, level_text);
    SDL_RenderDebugText(renderer, 10, 20, player_x);
    SDL_RenderDebugText(renderer, 10, 30, player_y);

    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    drawRectangle(renderer, w/2-200, 30, 400, 25);
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    drawRectangle(renderer, w/2-200, 30, player->progression_to_next_level/100*400, 25);

    SDL_RenderPresent(renderer);
}

void update(AppState *as, Uint64 dt_ns) {
    int w, h;
    if (!SDL_GetRenderOutputSize(as->renderer, &w, &h)) {
        return;
    }

    Player * player = as->player;
    float dt = dt_ns / 1e9f;

    updatePlayer(player, dt);
    updateProjectiles(as->projectiles, as->projectile_number, w, h, dt);

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

    SDL_LockMutex(as->dt_Mutex);
    as->dt_ns = dt_ns;
    SDL_UnlockMutex(as->dt_Mutex);

    update(as, as->dt_ns);

    draw(as);
    if (now - last > 999999999) {
        last = now;
        SDL_snprintf(debug_string, sizeof(debug_string), "%" SDL_PRIu64 " fps", accu);
        accu = 0;
    }
    past = now;
    accu += 1;
    Uint64 elapsed = SDL_GetTicksNS() - now;
    if (elapsed < 999999) {
        SDL_DelayNS(999999 - elapsed);
    }
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    AppState *as = (AppState *)appstate;
    as->running = 0;
    // Attendre que le thread termine
    SDL_WaitThread(as->enemyThread, NULL);

    // Détruire les mutex
    SDL_DestroyMutex(as->enemyMutex);
    SDL_DestroyMutex(as->dt_Mutex);

    SDL_free(as);
}