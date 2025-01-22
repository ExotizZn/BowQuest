#include "../include/fonts.h"

#include "../fonts/Poppins-Regular.h"
#include "../fonts/Poppins-Medium.h"
#include "../fonts/Poppins-SemiBold.h"

void loadFonts(Fonts * fonts) {
    SDL_IOStream *rw_poppins_regular = SDL_IOFromMem(Poppins_Regular_ttf, Poppins_Regular_ttf_len);
    SDL_IOStream *rw_poppins_medium = SDL_IOFromMem(Poppins_Medium_ttf, Poppins_Medium_ttf_len);
    SDL_IOStream *rw_poppins_semibold = SDL_IOFromMem(Poppins_SemiBold_ttf, Poppins_SemiBold_ttf_len);

    fonts->poppins_regular_10 = TTF_OpenFontIO(rw_poppins_regular, 0, 10);
    fonts->poppins_regular_11 = TTF_OpenFontIO(rw_poppins_regular, 0, 11);
    fonts->poppins_regular_12 = TTF_OpenFontIO(rw_poppins_regular, 0, 12);
    fonts->poppins_regular_13 = TTF_OpenFontIO(rw_poppins_regular, 0, 13);
    fonts->poppins_regular_14 = TTF_OpenFontIO(rw_poppins_regular, 0, 14);
    fonts->poppins_regular_15 = TTF_OpenFontIO(rw_poppins_regular, 0,  15);
    fonts->poppins_regular_16 = TTF_OpenFontIO(rw_poppins_regular, 1, 16);

    fonts->poppins_medium_10 = TTF_OpenFontIO(rw_poppins_medium, 0, 10);
    fonts->poppins_medium_11 = TTF_OpenFontIO(rw_poppins_medium, 0, 11);
    fonts->poppins_medium_12 = TTF_OpenFontIO(rw_poppins_medium, 0, 12);
    fonts->poppins_medium_13 = TTF_OpenFontIO(rw_poppins_medium, 0, 13);
    fonts->poppins_medium_14 = TTF_OpenFontIO(rw_poppins_medium, 0, 14);
    fonts->poppins_medium_15 = TTF_OpenFontIO(rw_poppins_medium, 0, 15);
    fonts->poppins_medium_16 = TTF_OpenFontIO(rw_poppins_medium, 1, 16);

    fonts->poppins_semibold_10 = TTF_OpenFontIO(rw_poppins_semibold, 0, 10);
    fonts->poppins_semibold_11 = TTF_OpenFontIO(rw_poppins_semibold, 0, 11);
    fonts->poppins_semibold_12 = TTF_OpenFontIO(rw_poppins_semibold, 0, 12);
    fonts->poppins_semibold_13 = TTF_OpenFontIO(rw_poppins_semibold, 0, 13);
    fonts->poppins_semibold_14 = TTF_OpenFontIO(rw_poppins_semibold, 0, 14);
    fonts->poppins_semibold_15 = TTF_OpenFontIO(rw_poppins_semibold, 0, 15);
    fonts->poppins_semibold_16 = TTF_OpenFontIO(rw_poppins_semibold, 1, 16);
}