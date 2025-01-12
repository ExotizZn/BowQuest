#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>

#include "../assets/Sprite-WhiteHand.h"

#define MAP_BOX_SCALE 16
#define MAP_BOX_EDGES_LEN (12 + MAP_BOX_SCALE * 2)
#define CIRCLE_DRAW_SIDES 32
#define CIRCLE_DRAW_SIDES_LEN (CIRCLE_DRAW_SIDES + 1)

typedef struct {
    SDL_MouseID mouse;
    SDL_KeyboardID keyboard;
    float x;
    float y;
    float level;
    unsigned char zqsd;
} Player;

typedef struct {
    float x;
    float y; 
} Camera;

typedef struct {
    float x, y;
    float dx, dy;
    bool active;
} Projectile;

typedef struct {
    float x, y;
    int w, h;
    SDL_Texture * image_texture;
} Ennemi;

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    Camera *camera;
    Player *player;
    Projectile * projectiles;
    Ennemi * ennemies;
} AppState;

static char debug_string[32];

void initPlayers(Player * player) {
    player->x = 0;
    player->y = 0;
    player->level = 0;
    player->zqsd = 0;
    player->mouse = 0;
    player->keyboard = 0;
}

SDL_Surface* CreateSurfaceFromMemory(const unsigned char* data, size_t size) {
    SDL_IOStream* rw = SDL_IOFromMem((void*)data, size);  // Crée un flux mémoire à partir des données
    if (!rw) {
        return NULL;
    }
    SDL_Surface* surface = IMG_Load_IO(rw, 1);  // Charge l'image en surface
    return surface;
}

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
    if (!SDL_CreateWindowAndRenderer("Archero C", 1024, 768, SDL_WINDOW_OPENGL, &as->window, &as->renderer)) {
        return SDL_APP_FAILURE;
    }

    as->camera = SDL_calloc(1, sizeof(Camera));
    as->player = SDL_calloc(1, sizeof(Player));
    as->projectiles = SDL_malloc(100*sizeof(Projectile));
    as->ennemies = SDL_calloc(1, sizeof(Ennemi));
    as->ennemies->x = 100;
    as->ennemies->y = 100;
    as->ennemies->w = 100;
    as->ennemies->h = 100;

    initPlayers(as->player);
    debug_string[0] = 0;

    SDL_SetHintWithPriority(SDL_HINT_WINDOWS_RAW_KEYBOARD, "1", SDL_HINT_OVERRIDE);

    return SDL_APP_CONTINUE;
}

void drawCircle(SDL_Renderer *renderer, float r, float x, float y) {
    float ang;
    SDL_FPoint points[CIRCLE_DRAW_SIDES_LEN];
    int i;
    for (i = 0; i < CIRCLE_DRAW_SIDES_LEN; i++) {
        ang = 2.0f * SDL_PI_F * (float)i / (float)CIRCLE_DRAW_SIDES;
        points[i].x = x + r * SDL_cosf(ang);
        points[i].y = y + r * SDL_sinf(ang);
    }
    SDL_RenderLines(renderer, (const SDL_FPoint*)&points, CIRCLE_DRAW_SIDES_LEN);
}

void drawRectangle(SDL_Renderer *renderer, int x, int y, int w, int h) {
    SDL_FRect rect = { x, y, w, h };
    SDL_RenderFillRect(renderer, &rect);
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
            break;
        }
        case SDL_EVENT_MOUSE_BUTTON_DOWN: {
            if(event->button.button == SDL_BUTTON_LEFT) {

            }
            break;
        }
    }
    return SDL_APP_CONTINUE;
}

void draw(AppState *as) { 
    SDL_Renderer * renderer = as->renderer;
    const Player * player = as->player;
    const Camera * camera = as->camera;
    //const Projectile * projectiles = as->projectiles;
    int w, h;
    if (!SDL_GetRenderOutputSize(renderer, &w, &h)) {
        return;
    }

    float mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);

    SDL_SetRenderDrawColor(renderer, 22, 22, 22, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderClipRect(renderer, 0);

    SDL_SetRenderDrawColor(renderer, 44, 44, 44, 255);

    int startX = ((int)camera->x / 50) * 50;
    if (camera->x < 0) startX -= 50;
    int startY = ((int)camera->y / 50) * 50;
    if (camera->y < 0) startY -= 50;

    int endX = camera->x + w;
    int endY = camera->y + h;

    // Dessiner les lignes verticales
    for (int x = startX; x <= endX; x += 50) {
        int screenX = x - camera->x;
        SDL_RenderLine(renderer, screenX, 0, screenX, h);
    }

    // Dessiner les lignes horizontales
    for (int y = startY; y <= endY; y += 50) {
        int screenY = y - camera->y;
        SDL_RenderLine(renderer, 0, screenY, w, screenY);
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDebugText(renderer, 10, 10, debug_string);

    static char player_x[32];
    static char player_y[32];

    SDL_snprintf(player_x, sizeof(player_x), "x: %.2f", player->x);
    SDL_snprintf(player_y, sizeof(player_y), "y: %.2f", player->y);

    SDL_RenderDebugText(renderer, 10, 20, player_x);
    SDL_RenderDebugText(renderer, 10, 30, player_y);

    static char level_text[32] = "Lvl. 0";
    SDL_RenderDebugText(renderer, w/2-20, 10, level_text);

    drawRectangle(renderer, w/2-25, h/2-25, 50, 50);

    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    drawRectangle(renderer, w/2-200, 30, 400, 25);
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    drawRectangle(renderer, w/2-200, 30, 200, 25);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderLine(renderer, w/2, h/2, mouse_x, mouse_y);

    SDL_Surface* image_surface = CreateSurfaceFromMemory(Sprite_WhiteHand_png, Sprite_WhiteHand_png_len);
    SDL_Texture *image_texture = SDL_CreateTextureFromSurface(renderer, image_surface);
    SDL_DestroySurface(image_surface);

    float im_w, im_h;

    SDL_GetTextureSize(image_texture, &im_w, &im_h);

    SDL_FRect dest_rect;
    dest_rect.x = as->ennemies->x-im_w/2 - camera->x;             
    dest_rect.y = as->ennemies->y-im_h/2 - camera->y;             
    dest_rect.w = im_w;         
    dest_rect.h = im_h;

    SDL_RenderTexture(renderer, image_texture, NULL, &dest_rect);

    SDL_RenderPresent(renderer);
}

void update(AppState *as, Uint64 dt_ns) {

    int w, h;
    if (!SDL_GetRenderOutputSize(as->renderer, &w, &h)) {
        return;
    }

    Player * player = as->player;

    int speed = 500;
    float dx = 0, dy = 0;
    float dt = dt_ns / 1e9f;

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

    // Applique le déplacement
    player->x += dx;
    player->y += dy;

    as->camera->x = player->x - w/2;
    as->camera->y = player->y - h/2;

    int speed_ennemi = 100;
    float ennemi_dx = 0, ennemi_dy = 0;

    if(as->ennemies->x < as->player->x-25) {
        ennemi_dx += speed_ennemi * dt;
    } else {
        ennemi_dx -= speed_ennemi * dt;
    }

    if(as->ennemies->y < as->player->y-25) {
        ennemi_dy += speed_ennemi * dt;
    } else {
        ennemi_dy -= speed_ennemi * dt;
    }

    float magnitude_ennemi = SDL_sqrtf(ennemi_dx * ennemi_dx + ennemi_dy * ennemi_dy);
    if (magnitude_ennemi > 0) {
        ennemi_dx = (ennemi_dx / magnitude_ennemi) * speed_ennemi * dt; // Normalise et applique la vitesse
        ennemi_dy = (ennemi_dy / magnitude_ennemi) * speed_ennemi * dt; // Normalise et applique la vitesse
    }

    as->ennemies->x += ennemi_dx;
    as->ennemies->y += ennemi_dy;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    AppState *as = appstate;
    static Uint64 accu = 0;
    static Uint64 last = 0;
    static Uint64 past = 0;
    Uint64 now = SDL_GetTicksNS();
    Uint64 dt_ns = now - past;
    update(as, dt_ns);
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
    SDL_free(appstate);
}
