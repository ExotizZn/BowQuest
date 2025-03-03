#include <SDL3/SDL.h>
#include "../include/appstate.h"
#include "../include/utils.h"
#include "../include/shop.h"
#include "../assets/shop/shop1.h"

static SDL_Texture *shop_texture = NULL;
static float shop_w = 1024, shop_h = 720;
static bool texture_loaded = false;

void drawShop(SDL_Renderer *renderer, AppState *as) {
    if (!renderer || !as) return;

    int window_w, window_h;
    SDL_GetRenderOutputSize(renderer, &window_w, &window_h);

    // Charger la texture si ce n’est pas encore fait
    if (!texture_loaded) {
        SDL_Surface *surface = CreateSurfaceFromMemory(Shop_png, Shop_png_len);
        if (surface) {
            shop_texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_DestroySurface(surface);
        }
        if (!shop_texture) {
            SDL_Log("Erreur lors du chargement de la texture du shop : %s", SDL_GetError());
        } else {
            SDL_GetTextureSize(shop_texture, &shop_w, &shop_h);
            texture_loaded = true;
        }
    }

    // Remplir la fenêtre tout en maintenant le ratio de l'image (1024x720)
    float image_ratio = 1024.0f / 720.0f;
    float window_ratio = (float)window_w / window_h;

    SDL_FRect dest_rect;

    if (window_w / 1024.0f < window_h / 720.0f) {
        dest_rect.w = window_w;
        dest_rect.h = window_w / image_ratio;
    } else {
        dest_rect.h = window_h;
        dest_rect.w = window_h * image_ratio;
    }

    // Centrer l'image dans la fenêtre
    dest_rect.x = (window_w - dest_rect.w) / 2;
    dest_rect.y = (window_h - dest_rect.h) / 2;

    if (shop_texture) {
        SDL_RenderTexture(renderer, shop_texture, NULL, &dest_rect);
    }

    // Calculer les échelles pour adapter aux dimensions de la fenêtre
    float scale_x = dest_rect.w / 1024.0f;  // Rapport largeur
    float scale_y = dest_rect.h / 720.0f;   // Rapport hauteur

    // Zones cliquables pour la navigation (placées en bas, comme dans l’image)
    // Bouton "SHOP" (en bas à gauche, x=0, y=670 dans 1024x720)
    SDL_FRect shop_btn = {
        .x = dest_rect.x,  // Position gauche
        .y = dest_rect.y + dest_rect.h - (50 * scale_y),  // En bas (50 pixels du bas)
        .w = 341 * scale_x,  // Largeur ajustée (comme les boutons du menu)
        .h = 50 * scale_y    // Hauteur ajustée
    };

    // Bouton "GAME" (en bas au milieu, x=341, y=670 dans 1024x720, page 0)
    SDL_FRect game_btn = {
        .x = dest_rect.x + (341 * scale_x),  // Position milieu
        .y = dest_rect.y + dest_rect.h - (50 * scale_y),  // En bas (50 pixels du bas)
        .w = 341 * scale_x,  // Largeur ajustée
        .h = 50 * scale_y    // Hauteur ajustée
    };

    // Bouton "EQUIPMENT" (en bas à droite, x=683, y=670 dans 1024x720)
    SDL_FRect equipment_btn = {
        .x = dest_rect.x + (683 * scale_x),  // Position droite
        .y = dest_rect.y + dest_rect.h - (50 * scale_y),  // En bas (50 pixels du bas)
        .w = 341 * scale_x,  // Largeur ajustée
        .h = 50 * scale_y    // Hauteur ajustée
    };

    // Vérification de débogage pour les clics
    SDL_Log("Mouse: x=%d, y=%d, left_button=%d", (int)as->mouse->x, (int)as->mouse->y, as->mouse->left_button);
    SDL_Log("Shop btn: x=%f, y=%f, w=%f, h=%f", shop_btn.x, shop_btn.y, shop_btn.w, shop_btn.h);
    SDL_Log("Game btn: x=%f, y=%f, w=%f, h=%f", game_btn.x, game_btn.y, game_btn.w, game_btn.h);
    SDL_Log("Equipment btn: x=%f, y=%f, w=%f, h=%f", equipment_btn.x, equipment_btn.y, equipment_btn.w, equipment_btn.h);

    // Vérifier les clics
    bool click_shop = as->mouse->x >= shop_btn.x && as->mouse->x <= (shop_btn.x + shop_btn.w) &&
                      as->mouse->y >= shop_btn.y && as->mouse->y <= (shop_btn.y + shop_btn.h) &&
                      as->mouse->left_button;

    bool click_game = as->mouse->x >= game_btn.x && as->mouse->x <= (game_btn.x + game_btn.w) &&
                      as->mouse->y >= game_btn.y && as->mouse->y <= (game_btn.y + game_btn.h) &&
                      as->mouse->left_button;

    bool click_equipment = as->mouse->x >= equipment_btn.x && as->mouse->x <= (equipment_btn.x + equipment_btn.w) &&
                          as->mouse->y >= equipment_btn.y && as->mouse->y <= (equipment_btn.y + equipment_btn.h) &&
                          as->mouse->left_button;

    if (click_shop) {
        SDL_Log("Clic sur Shop détecté");
        as->page = 2;  // Rester sur SHOP (redondant, mais pour cohérence)
        as->mouse->left_button = false;  // Réinitialise le clic
    }
    if (click_game) {
        SDL_Log("Clic sur Game détecté");
        as->page = 0;  // Passer à la page GAME (maintenant page 0)
        as->mouse->left_button = false;  // Réinitialise le clic
    }
    if (click_equipment) {
        SDL_Log("Clic sur Equipment détecté");
        as->page = 3;  // Passer à la page EQUIPMENT
        as->mouse->left_button = false;  // Réinitialise le clic
    }
}