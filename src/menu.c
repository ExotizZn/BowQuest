#include "../include/appstate.h"
#include "../include/utils.h"
#include "../include/menu.h"

#include "../assets/menu/background.h"
#include "../assets/menu/menu.h"
#include "../assets/menu/shop.h"
#include "../assets/menu/equipment.h"

static SDL_Texture *background_texture = NULL;
static SDL_Texture *menu_texture = NULL;
static SDL_Texture *shop_texture = NULL;
static SDL_Texture *equipment_texture = NULL;

static float menu_w = 0, menu_h = 0;
static float background_w = 0, background_h = 0;
static bool texture_loaded = false;
static int page = 0;

typedef struct {
    SDL_FRect *rect;
    const char *name;
    int page;
} Button;

void drawMenu(SDL_Renderer *renderer, AppState *as) {
    if (!renderer || !as) return;

    int window_w, window_h;
    SDL_GetRenderOutputSize(renderer, &window_w, &window_h);

    if (!texture_loaded) {
        SDL_Surface *surface_background = CreateSurfaceFromMemory(background_png, background_png_len);
        SDL_Surface *surface_menu = CreateSurfaceFromMemory(menu_png, menu_png_len);
        SDL_Surface *surface_shop = CreateSurfaceFromMemory(shop_png, shop_png_len);
        SDL_Surface *surface_equipment = CreateSurfaceFromMemory(equipment_png, equipment_png_len);

        background_texture = SDL_CreateTextureFromSurface(renderer, surface_background);
        menu_texture = SDL_CreateTextureFromSurface(renderer, surface_menu);
        shop_texture = SDL_CreateTextureFromSurface(renderer, surface_shop);
        equipment_texture = SDL_CreateTextureFromSurface(renderer, surface_equipment);
        SDL_DestroySurface(surface_background);
        SDL_DestroySurface(surface_menu);
        SDL_DestroySurface(surface_shop);
        SDL_DestroySurface(surface_equipment);
    
        SDL_GetTextureSize(background_texture, &background_w, &background_h);
        SDL_GetTextureSize(menu_texture, &menu_w, &menu_h);
        texture_loaded = true;
    }

    float background_ratio = background_w / background_h;
    SDL_FRect background_dest;

    float image_ratio = menu_w / menu_h;
    float window_ratio = (float)window_w / window_h;

    SDL_FRect dest_rect;

    dest_rect.w = (window_w / menu_w < window_h / menu_h) ? window_w : window_h * image_ratio;
    dest_rect.h = (window_w / menu_w < window_h / menu_h) ? window_w / image_ratio : window_h;

    background_dest.w = (window_w / menu_w < window_h / menu_h) ? window_w : window_h * background_ratio;
    background_dest.h = (window_w / menu_w < window_h / menu_h) ? window_w / background_ratio : window_h;
    
    dest_rect.x = (window_w - dest_rect.w) / 2;
    dest_rect.y = (window_h - dest_rect.h) / 2;

    background_dest.x = (window_w - background_dest.w) / 2;
    background_dest.y = (window_h - background_dest.h) / 2;

    switch(page) {
    case 0:
        SDL_RenderTexture(renderer, menu_texture, NULL, &dest_rect);
        break;
    case 1:
        SDL_RenderTexture(renderer, shop_texture, NULL, &dest_rect);
        break;
    case 2:
        SDL_RenderTexture(renderer, equipment_texture, NULL, &dest_rect);
        break;
    }

    //SDL_RenderTexture(renderer, background_texture, NULL, &background_dest);

    float scale_x = dest_rect.w / menu_w;
    float scale_y = dest_rect.h / menu_h;

    SDL_FRect shop_rect = {
        .x = dest_rect.x,
        .y = dest_rect.y + dest_rect.h - 100,
        .w = window_w/3,
        .h = 100
    };

    SDL_FRect game_rect = {
        .x = dest_rect.x + window_w/3,
        .y = dest_rect.y + dest_rect.h - 100,
        .w = window_w/3,
        .h = 100
    };

    SDL_FRect equipment_rect = {
        .x = dest_rect.x + 2 * window_w/3,
        .y = dest_rect.y + dest_rect.h - 100,
        .w = window_w/3,
        .h = 100
    };

    Button buttons[] = {
        { &game_rect, "GAME", 0 },
        { &shop_rect, "SHOP", 1 },
        { &equipment_rect, "EQUIPMENT", 2 }
    };

    for (int i = 0; i < 3; i++) {
        bool mouse_over = as->mouse->x >= buttons[i].rect->x && as->mouse->x <= (buttons[i].rect->x + buttons[i].rect->w) &&
                          as->mouse->y >= buttons[i].rect->y && as->mouse->y <= (buttons[i].rect->y + buttons[i].rect->h);

        if (mouse_over && as->mouse->left_button) {
            page = buttons[i].page;
            break;
        }
    }

    SDL_FRect jouer_rect = {
        .x = (window_w - (200 * (dest_rect.w / 1024.0f))) / 2,
        .y = (window_h - (50 * (dest_rect.h / 720.0f))) / 2,
        .w = 200 * (dest_rect.w / 1024.0f),
        .h = 50 * (dest_rect.h / 720.0f)
    };

    bool clicJouer = as->mouse->x >= jouer_rect.x && as->mouse->x <= (jouer_rect.x + jouer_rect.w) &&
                     as->mouse->y >= jouer_rect.y && as->mouse->y <= (jouer_rect.y + jouer_rect.h);

    if (clicJouer && as->mouse->left_button) {
        as->page = 1; 
    }
}