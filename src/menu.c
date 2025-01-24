#include "../include/appstate.h"
#include "../include/utils.h"
#include "../include/menu.h"

void drawMenu(void * data, int w, int h) {
    AppState * as = (AppState*) data;
    for(int i = 0; i < 3; i++){
        if(i == 0) {
            SDL_SetRenderDrawColor(as->renderer, 255, 0, 0, 255);
        } else if (i == 1) {
            SDL_SetRenderDrawColor(as->renderer, 0, 255, 0, 255);
        } else {
            SDL_SetRenderDrawColor(as->renderer, 0, 0, 255, 255);
        }
        fillRect(as->renderer, (w/3)*i, h-100, w/3, 100);
        fillRect(as->renderer, (w/3)*i, 0, w/3, 100);
    }
}