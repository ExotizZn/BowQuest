#include <SDL3/SDL.h>
#include "../include/appstate.h"
#include "../include/utils.h"
#include "../include/upgrade.h"

#include "../assets/upgrade/boost_atk.h"
#include "../assets/upgrade/double_arrows.h"
#include "../assets/upgrade/flaming_arrows.h"
#include "../assets/upgrade/frozen_arrows.h"
#include "../assets/upgrade/rage.h"
#include "../assets/upgrade/rapid_fire.h"
#include "../assets/upgrade/self_healing.h"
#include "../assets/upgrade/shield.h"
#include "../assets/upgrade/toxic_arrows.h"

#define NUM_UPGRADE_ITEMS 8

// Define the array with the named type
static UpgradeAsset upgrade_assets[NUM_UPGRADE_ITEMS];

// Helper function to load a texture safely
static SDL_Texture *loadTexture(SDL_Renderer *renderer, const unsigned char *data, size_t len, const char *name) {
    SDL_Surface *surface = CreateSurfaceFromMemory(data, len);
    if (!surface) {
        SDL_Log("Failed to create surface for %s: %s", name, SDL_GetError());
        return NULL;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    if (!texture) {
        SDL_Log("Failed to create texture for %s: %s", name, SDL_GetError());
    }
    return texture;
}

void loadItemsAssets(void *data, SDL_Texture **textures) {
    AppState *as = (AppState *)data;
    if (!as || !as->renderer || !textures) {
        SDL_Log("Invalid parameters in loadItemsAssets");
        return;
    }

    // Populate the array at runtime
    static bool initialized = false;
    if (!initialized) {
        upgrade_assets[0]  = (UpgradeAsset){boost_atk_png, boost_atk_png_len, "Attaque boosté", "Ajoute 10%% d'attaque."};
        upgrade_assets[1]  = (UpgradeAsset){double_arrows_png, double_arrows_png_len, "Flèches doubles", "Double les flèches."};
        upgrade_assets[2]  = (UpgradeAsset){flaming_arrows_png, flaming_arrows_png_len, "Flèches enflamés", "Enflamme les ennemis pendant une courte période."};
        upgrade_assets[3]  = (UpgradeAsset){frozen_arrows_png, frozen_arrows_png_len, "Flèches gelés", "Gèle l'ennemi touché pendant une courte période.'"};
        upgrade_assets[4] = (UpgradeAsset){rapid_fire_png, rapid_fire_png_len, "Tir rapide", "Améliore le delai entre les tirs."};
        upgrade_assets[5] = (UpgradeAsset){self_healing_png, self_healing_png_len, "Soin passif", "Soigne le joueur passivement."};
        upgrade_assets[6] = (UpgradeAsset){shield_png, shield_png_len, "Armure", "Ajoute de l'armure au joueur."};
        upgrade_assets[7] = (UpgradeAsset){toxic_arrows_png, toxic_arrows_png_len, "Flèches toxiques", "Empoisone l'ennemi touché pendant une courte période."};
        initialized = true;
    }

    for (int i = 0; i < NUM_UPGRADE_ITEMS; i++) {
        textures[i] = loadTexture(as->renderer, upgrade_assets[i].data, upgrade_assets[i].len, upgrade_assets[i].name);
    }
}

UpgradeAsset *drawItem(void *data, float x, float y, float w, float h, int item_id) {
    AppState *as = (AppState *)data;
    if (!as || !as->renderer || item_id < 0 || item_id >= NUM_UPGRADE_ITEMS || !as->skills_assets[item_id]) return NULL;

    SDL_FRect dest_rect = {x, y, w, h};
    SDL_RenderTextureRotated(as->renderer, as->skills_assets[item_id], NULL, &dest_rect, 0, NULL, SDL_FLIP_NONE);

    return &upgrade_assets[item_id];
}