#include "../include/appstate.h"
#include "../include/utils.h"
#include "../include/menu.h"
#include "../assets/menu/menu1.h"

static SDL_Texture *menu_texture = NULL;
static float menu_w = 1024, menu_h = 720;
static bool texture_loaded = false;

void drawMenu(SDL_Renderer *renderer, AppState *as) {
    if (!renderer || !as) return;

    int window_w, window_h;
    SDL_GetRenderOutputSize(renderer, &window_w, &window_h);

    // Charger la texture si ce n’est pas encore fait
    if (!texture_loaded) {
        SDL_Surface *surface = CreateSurfaceFromMemory(Menu_png, Menu_png_len);
        if (surface) {
            menu_texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_DestroySurface(surface);
        }
        if (!menu_texture) {
            SDL_Log("Erreur lors du chargement de la texture du menu : %s", SDL_GetError());
        } else {
            SDL_GetTextureSize(menu_texture, &menu_w, &menu_h);
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

    if (menu_texture) {
        SDL_RenderTexture(renderer, menu_texture, NULL, &dest_rect);
    }

    // Calculer les échelles pour adapter aux dimensions de la fenêtre
    float scale_x = dest_rect.w / 1024.0f;  // Rapport largeur
    float scale_y = dest_rect.h / 720.0f;   // Rapport hauteur

    // Définir les zones cliquables pour les boutons en bas (SHOP, GAME, EQUIPMENT)
    // Boutons à y=670 (50 pixels du bas), chacun de 341x50 pixels
    SDL_FRect shop_rect = {
        .x = dest_rect.x,                          // Tout à gauche
        .y = dest_rect.y + dest_rect.h - (50 * scale_y),  // Position en bas (50 pixels du bas)
        .w = 341 * scale_x,                        // Largeur = 1/3 de l’image
        .h = 50 * scale_y                          // Hauteur ajustée
    };

    SDL_FRect game_rect = {
        .x = dest_rect.x + (341 * scale_x),        // Centre
        .y = dest_rect.y + dest_rect.h - (50 * scale_y),  // Position en bas
        .w = 341 * scale_x,                        // Largeur = 1/3
        .h = 50 * scale_y                          // Hauteur ajustée
    };

    SDL_FRect equipment_rect = {
        .x = dest_rect.x + (683 * scale_x),        // Tout à droite
        .y = dest_rect.y + dest_rect.h - (50 * scale_y),  // Position en bas
        .w = 341 * scale_x,                        // Largeur = 1/3
        .h = 50 * scale_y                          // Hauteur ajustée
    };

    // Vérifier les clics sur les boutons du bas
    struct {
        SDL_FRect *rect;
        const char *name;
        int page;  // Page à changer
    } buttons[] = {
        { &shop_rect, "SHOP", 2 },          // Passe à la page 2 pour SHOP
        { &game_rect, "GAME", 0 },          // Passe à la page 0 pour GAME (menu principal)
        { &equipment_rect, "EQUIPMENT", 3 }  // Passe à la page 3 pour EQUIPMENT
    };

    for (int i = 0; i < 3; i++) {
        bool mouse_over = as->mouse->x >= buttons[i].rect->x && as->mouse->x <= (buttons[i].rect->x + buttons[i].rect->w) &&
                          as->mouse->y >= buttons[i].rect->y && as->mouse->y <= (buttons[i].rect->y + buttons[i].rect->h);

        if (mouse_over && as->mouse->left_button) {
            as->page = buttons[i].page;  // Change la page selon le bouton cliqué
            as->mouse->left_button = false;  // Réinitialise le clic
            break;  // Sortir de la boucle après un clic pour éviter des conflits
        }
    }

    // Définir la zone cliquable pour le bouton "JOUER"
    SDL_FRect jouer_rect = {
        .x = (window_w - (200 * (dest_rect.w / 1024.0f))) / 2,  // Centre horizontalement, ajusté à l’échelle
        .y = (window_h - (50 * (dest_rect.h / 720.0f))) / 2,    // Centre verticalement, ajusté à l’échelle
        .w = 200 * (dest_rect.w / 1024.0f),                     // Largeur ajustée
        .h = 50 * (dest_rect.h / 720.0f)                        // Hauteur ajustée
    };

    // Vérifier si la souris clique sur "JOUER"
    bool clicJouer = as->mouse->x >= jouer_rect.x && as->mouse->x <= (jouer_rect.x + jouer_rect.w) &&
                     as->mouse->y >= jouer_rect.y && as->mouse->y <= (jouer_rect.y + jouer_rect.h);

    if (clicJouer && as->mouse->left_button) {
        as->page = 1;  // Passe à la page 1 pour JOUER
        as->mouse->left_button = false;  // Réinitialise le clic
    }
}