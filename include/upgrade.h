#ifndef UPGRADE_H
#define UPGRADE_H

typedef struct {
    const unsigned char *data;
    size_t len;
    const char *name;
    const char *description;
} UpgradeAsset;

void loadItemsAssets(void *data, SDL_Texture **textures);
UpgradeAsset *drawItem(void *data, float x, float y, float w, float h, int item_id);
void cleanupUpgradeMenuCursors(void);

#endif