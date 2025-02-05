#ifndef UPGRADE_H
#define UPGRADE_H

typedef struct {    
    int id;
    char * name;
    char * description;
} Upgrade;

void loadItemsAssets(void * data, SDL_Texture ** textures);
void drawItem(void * data, float x, float y, float w, float h, int item_id);

#endif