#ifndef ASSET_LOADER_H
#define ASSET_LOADER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include "util.h"
#include "COIExtraCollision.h"

typedef struct COIAssetLoader {
  SDL_Surface** _assets;
  int _assetCount;
  COIExtraCollision** _extraCollisions;
}COIAssetLoader;

COIAssetLoader* COIAssetLoaderCreate();
void COIAssetLoaderDestroy(COIAssetLoader* loader);

SDL_Surface* COIAssetLoaderGetAsset(COIAssetLoader* loader, int index);
COIExtraCollision* COIAssetLoaderGetCollision(COIAssetLoader*, int index);

#endif
