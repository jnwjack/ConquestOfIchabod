#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "util.h"

#ifndef ASSET_LOADER_H
#define ASSET_LOADER_H

typedef struct COIAssetLoader {
  SDL_Surface** _assets;
  int _assetCount;
}COIAssetLoader;

COIAssetLoader* COIAssetLoaderCreate(const char* filename);
void COIAssetLoaderDestroy(COIAssetLoader* loader);

SDL_Surface* COIAssetLoaderGetAsset(COIAssetLoader* loader, int index);

#endif
