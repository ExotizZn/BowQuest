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

void loadItemsAssets(void * data, SDL_Texture ** textures) {
    AppState * as = (AppState *)data;

    textures[0] = SDL_CreateTextureFromSurface(as->renderer, CreateSurfaceFromMemory(better_chances_png, better_chances_png_len));
    textures[1] = SDL_CreateTextureFromSurface(as->renderer, CreateSurfaceFromMemory(boost_atk_png, boost_atk_png_len));
    textures[2] = SDL_CreateTextureFromSurface(as->renderer, CreateSurfaceFromMemory(critical_arrows_png, critical_arrows_png_len));
    textures[3] = SDL_CreateTextureFromSurface(as->renderer, CreateSurfaceFromMemory(double_arrows_png, double_arrows_png_len));
    textures[4] = SDL_CreateTextureFromSurface(as->renderer, CreateSurfaceFromMemory(elementary_arrows_png, elementary_arrows_png_len));
    textures[5] = SDL_CreateTextureFromSurface(as->renderer, CreateSurfaceFromMemory(explosive_arrows_png, explosive_arrows_png_len));
    textures[6] = SDL_CreateTextureFromSurface(as->renderer, CreateSurfaceFromMemory(fast_walking_png, fast_walking_png_len));
    textures[7] = SDL_CreateTextureFromSurface(as->renderer, CreateSurfaceFromMemory(flaming_arrows_png, flaming_arrows_png_len));
    textures[8] = SDL_CreateTextureFromSurface(as->renderer, CreateSurfaceFromMemory(frozen_arrows_png, frozen_arrows_png_len));
    textures[9] = SDL_CreateTextureFromSurface(as->renderer, CreateSurfaceFromMemory(ice_explosion_png, ice_explosion_png_len));
    textures[10] = SDL_CreateTextureFromSurface(as->renderer, CreateSurfaceFromMemory(increased_loot_png, increased_loot_png_len));
    textures[11] = SDL_CreateTextureFromSurface(as->renderer, CreateSurfaceFromMemory(life_steal_png, life_steal_png_len));
    textures[12] = SDL_CreateTextureFromSurface(as->renderer, CreateSurfaceFromMemory(offensive_dash_png, offensive_dash_png_len));
    textures[13] = SDL_CreateTextureFromSurface(as->renderer, CreateSurfaceFromMemory(piercing_arrows_png, piercing_arrows_png_len));
    textures[14] = SDL_CreateTextureFromSurface(as->renderer, CreateSurfaceFromMemory(rage_png, rage_png_len));
    textures[15] = SDL_CreateTextureFromSurface(as->renderer, CreateSurfaceFromMemory(rapid_fire_png, rapid_fire_png_len));
    textures[16] = SDL_CreateTextureFromSurface(as->renderer, CreateSurfaceFromMemory(ricochet_arrows_png, ricochet_arrows_png_len));
    textures[17] = SDL_CreateTextureFromSurface(as->renderer, CreateSurfaceFromMemory(rune_powers_png, rune_powers_png_len));
    textures[18] = SDL_CreateTextureFromSurface(as->renderer, CreateSurfaceFromMemory(self_healing_png, self_healing_png_len));
    textures[19] = SDL_CreateTextureFromSurface(as->renderer, CreateSurfaceFromMemory(shield_png, shield_png_len));
    textures[20] = SDL_CreateTextureFromSurface(as->renderer, CreateSurfaceFromMemory(thorny_shield_png, thorny_shield_png_len));
    textures[21] = SDL_CreateTextureFromSurface(as->renderer, CreateSurfaceFromMemory(toxic_arrows_png, toxic_arrows_png_len));
    textures[22] = SDL_CreateTextureFromSurface(as->renderer, CreateSurfaceFromMemory(trap_png, trap_png_len));
}

void drawItem(void * data, float x, float y, float w, float h, int item_id) {
    AppState * as = (AppState *)data;
    SDL_Renderer * renderer = as->renderer;

    SDL_FRect dest_rect = {
        .x = x,
        .y = y,
        .w = w,
        .h = h
    };

    SDL_RenderTextureRotated(renderer, as->skills_assets[item_id], NULL, &dest_rect, 0, NULL, SDL_FLIP_NONE);
}