#ifndef ASSET_LOADER_H
#define ASSET_LOADER_H

#ifdef __WINDOWS__
#define SDL_MAIN_HANDLED
#endif
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
void COIAssetLoaderInit();

extern COIAssetLoader* COI_GLOBAL_LOADER;

#endif
