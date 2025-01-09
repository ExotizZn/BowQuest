#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define MAP_BOX_SCALE 16
#define MAP_BOX_EDGES_LEN (12 + MAP_BOX_SCALE * 2)
#define CIRCLE_DRAW_SIDES 32
#define CIRCLE_DRAW_SIDES_LEN (CIRCLE_DRAW_SIDES + 1)

typedef struct {
    SDL_MouseID mouse;
    SDL_KeyboardID keyboard;
    int x;
    int y;
    unsigned char zqsd;
} Player;

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    Player *player;
} AppState;

static char debug_string[32];

static void initPlayers(Player *player) {
    player->x = 0;
    player->y = 0;
    player->zqsd = 0;
    player->mouse = 0;
    player->keyboard = 0;
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
    if (!SDL_CreateWindowAndRenderer("Archero C", 1024, 768, 0, &as->window, &as->renderer)) {
        return SDL_APP_FAILURE;
    }

    as->player = SDL_calloc(1, sizeof(Player));
    initPlayers(as->player);
    debug_string[0] = 0;

    SDL_SetHintWithPriority(SDL_HINT_WINDOWS_RAW_KEYBOARD, "1", SDL_HINT_OVERRIDE);
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    AppState *as = appstate;
    Player *player = as->player;
    switch (event->type) {
        case SDL_EVENT_KEY_UP: {
            SDL_Keycode sym = event->key.key;
            SDL_KeyboardID id = event->key.which;
            if (sym == SDLK_ESCAPE) return SDL_APP_SUCCESS;
            if (sym == SDLK_Z) player->zqsd &= ~0x01; // Z : bit 0
            if (sym == SDLK_Q) player->zqsd &= ~0x02; // Q : bit 1
            if (sym == SDLK_S) player->zqsd &= ~0x04; // S : bit 2
            if (sym == SDLK_D) player->zqsd &= ~0x08; // D : bit 3
            break;
        }
        case SDL_EVENT_KEY_DOWN: {
            SDL_Keycode sym = event->key.key;
            SDL_KeyboardID id = event->key.which;
            if (sym == SDLK_Z) player->zqsd |= 0x01; // Z : bit 0
            if (sym == SDLK_Q) player->zqsd |= 0x02; // Q : bit 1
            if (sym == SDLK_S) player->zqsd |= 0x04; // S : bit 2
            if (sym == SDLK_D) player->zqsd |= 0x08; // D : bit 3
            break;
        }
    }
    return SDL_APP_CONTINUE;
}

static void drawCircle(SDL_Renderer *renderer, float r, float x, float y) {
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
    // Définir la couleur du rectangle (rouge, vert, bleu, alpha)
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    // Définir les dimensions et la position du rectangle
    SDL_FRect rect = { x, y, w, h }; // x, y, largeur, hauteur

    // Dessiner le rectangle rempli
    SDL_RenderFillRect(renderer, &rect);
}

static void draw(SDL_Renderer *renderer, const Player * player) { 
    int w, h;
    if (!SDL_GetRenderOutputSize(renderer, &w, &h)) {
        return;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_SetRenderClipRect(renderer, 0);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDebugText(renderer, 10, 10, debug_string);

    drawCircle(renderer, 50, w/2, h/2);
    drawRectangle(renderer, player->x, player->y, 50, 50);

    SDL_RenderPresent(renderer);
}

static void update(Player *player, Uint64 dt_ns) {
    int speed = 5;
    float dx = 0, dy = 0;

    // Calcule les directions en fonction des touches enfoncées
    if (player->zqsd & 0x01) dy -= 1; // Z (haut)
    if (player->zqsd & 0x02) dx -= 1; // Q (gauche)
    if (player->zqsd & 0x04) dy += 1; // S (bas)
    if (player->zqsd & 0x08) dx += 1; // D (droite)

    // Calcule la magnitude uniquement si un déplacement est requis
    float magnitude = SDL_sqrtf(dx * dx + dy * dy);
    if (magnitude > 0) {
        dx = (dx / magnitude) * speed; // Normalise et applique la vitesse
        dy = (dy / magnitude) * speed; // Normalise et applique la vitesse
    }

    // Applique le déplacement
    player->x += (int)dx;
    player->y += (int)dy;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    AppState *as = appstate;
    static Uint64 accu = 0;
    static Uint64 last = 0;
    static Uint64 past = 0;
    Uint64 now = SDL_GetTicksNS();
    Uint64 dt_ns = now - past;
    update(as->player, dt_ns);
    draw(as->renderer, as->player);
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
