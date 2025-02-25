#ifndef FONTS_H
#define FONTS_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

typedef struct {
    // Poppins
    TTF_Font *poppins_regular_10;
    TTF_Font *poppins_regular_11;
    TTF_Font *poppins_regular_12;
    TTF_Font *poppins_regular_13;
    TTF_Font *poppins_regular_14;
    TTF_Font *poppins_regular_15;
    TTF_Font *poppins_regular_16;

    TTF_Font *poppins_medium_10;
    TTF_Font *poppins_medium_11;
    TTF_Font *poppins_medium_12;
    TTF_Font *poppins_medium_13;
    TTF_Font *poppins_medium_14;
    TTF_Font *poppins_medium_15;
    TTF_Font *poppins_medium_16;

    TTF_Font *poppins_semibold_10;
    TTF_Font *poppins_semibold_11;
    TTF_Font *poppins_semibold_12;
    TTF_Font *poppins_semibold_13;
    TTF_Font *poppins_semibold_14;
    TTF_Font *poppins_semibold_15;
    TTF_Font *poppins_semibold_16;
} Fonts;

void loadFonts(Fonts *fonts);

#endif