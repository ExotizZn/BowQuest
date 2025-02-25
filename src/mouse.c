#include "../include/appstate.h"

void initMouse(Mouse **mouse) {
    *mouse = SDL_calloc(1, sizeof(Mouse));

    (*mouse)->x = 0;
    (*mouse)->y = 0;
    (*mouse)->left_button = false;
    (*mouse)->right_button = false;
}