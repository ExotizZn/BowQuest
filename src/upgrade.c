#include <SDL3/SDL.h>
#include "../include/appstate.h"
#include "../include/utils.h"
#include "../include/upgrade.h"
#include "../assets/upgrade/better_chances.h"
#include "../assets/upgrade/boost_atk.h"
#include "../assets/upgrade/critical_arrows.h"
#include "../assets/upgrade/double_arrows.h"
#include "../assets/upgrade/elementary_arrows.h"
#include "../assets/upgrade/explosive_arrows.h"
#include "../assets/upgrade/fast_walking.h"
#include "../assets/upgrade/flaming_arrows.h"
#include "../assets/upgrade/frozen_arrows.h"
#include "../assets/upgrade/ice_explosion.h"
#include "../assets/upgrade/increased_loot.h"
#include "../assets/upgrade/life_steal.h"
#include "../assets/upgrade/offensive_dash.h"
#include "../assets/upgrade/piercing_arrows.h"
#include "../assets/upgrade/rage.h"
#include "../assets/upgrade/rapid_fire.h"
#include "../assets/upgrade/ricochet_arrows.h"
#include "../assets/upgrade/rune_powers.h"
#include "../assets/upgrade/self_healing.h"
#include "../assets/upgrade/shield.h"
#include "../assets/upgrade/thorny_shield.h"
#include "../assets/upgrade/toxic_arrows.h"
#include "../assets/upgrade/trap.h"

#define NUM_UPGRADE_ITEMS 23

// Define a named struct type
typedef struct {
    const unsigned char *data;
    size_t len;
    const char *name;
} UpgradeAsset;

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
        upgrade_assets[0]  = (UpgradeAsset){better_chances_png, better_chances_png_len, "better_chances"};
        upgrade_assets[1]  = (UpgradeAsset){boost_atk_png, boost_atk_png_len, "boost_atk"};
        upgrade_assets[2]  = (UpgradeAsset){critical_arrows_png, critical_arrows_png_len, "critical_arrows"};
        upgrade_assets[3]  = (UpgradeAsset){double_arrows_png, double_arrows_png_len, "double_arrows"};
        upgrade_assets[4]  = (UpgradeAsset){elementary_arrows_png, elementary_arrows_png_len, "elementary_arrows"};
        upgrade_assets[5]  = (UpgradeAsset){explosive_arrows_png, explosive_arrows_png_len, "explosive_arrows"};
        upgrade_assets[6]  = (UpgradeAsset){fast_walking_png, fast_walking_png_len, "fast_walking"};
        upgrade_assets[7]  = (UpgradeAsset){flaming_arrows_png, flaming_arrows_png_len, "flaming_arrows"};
        upgrade_assets[8]  = (UpgradeAsset){frozen_arrows_png, frozen_arrows_png_len, "frozen_arrows"};
        upgrade_assets[9]  = (UpgradeAsset){ice_explosion_png, ice_explosion_png_len, "ice_explosion"};
        upgrade_assets[10] = (UpgradeAsset){increased_loot_png, increased_loot_png_len, "increased_loot"};
        upgrade_assets[11] = (UpgradeAsset){life_steal_png, life_steal_png_len, "life_steal"};
        upgrade_assets[12] = (UpgradeAsset){offensive_dash_png, offensive_dash_png_len, "offensive_dash"};
        upgrade_assets[13] = (UpgradeAsset){piercing_arrows_png, piercing_arrows_png_len, "piercing_arrows"};
        upgrade_assets[14] = (UpgradeAsset){rage_png, rage_png_len, "rage"};
        upgrade_assets[15] = (UpgradeAsset){rapid_fire_png, rapid_fire_png_len, "rapid_fire"};
        upgrade_assets[16] = (UpgradeAsset){ricochet_arrows_png, ricochet_arrows_png_len, "ricochet_arrows"};
        upgrade_assets[17] = (UpgradeAsset){rune_powers_png, rune_powers_png_len, "rune_powers"};
        upgrade_assets[18] = (UpgradeAsset){self_healing_png, self_healing_png_len, "self_healing"};
        upgrade_assets[19] = (UpgradeAsset){shield_png, shield_png_len, "shield"};
        upgrade_assets[20] = (UpgradeAsset){thorny_shield_png, thorny_shield_png_len, "thorny_shield"};
        upgrade_assets[21] = (UpgradeAsset){toxic_arrows_png, toxic_arrows_png_len, "toxic_arrows"};
        upgrade_assets[22] = (UpgradeAsset){trap_png, trap_png_len, "trap"};
        initialized = true;
    }

    for (int i = 0; i < NUM_UPGRADE_ITEMS; i++) {
        textures[i] = loadTexture(as->renderer, upgrade_assets[i].data, upgrade_assets[i].len, upgrade_assets[i].name);
    }
}

void drawItem(void *data, float x, float y, float w, float h, int item_id) {
    AppState *as = (AppState *)data;
    if (!as || !as->renderer || item_id < 0 || item_id >= NUM_UPGRADE_ITEMS || !as->skills_assets[item_id]) return;

    SDL_FRect dest_rect = {x, y, w, h};
    SDL_RenderTextureRotated(as->renderer, as->skills_assets[item_id], NULL, &dest_rect, 0, NULL, SDL_FLIP_NONE);
}