#include "COIAssetLoader.h"

COIAssetLoader* COIAssetLoaderCreate(const char* filename) {
  FILE* fp;
  char* line;
  size_t len = 0;
  int assetCount = countLines(filename);
  COIAssetLoader* loader = malloc(sizeof(COIAssetLoader));
  SDL_Surface** surfaces = malloc(assetCount * sizeof(SDL_Surface*));

  fp = fopen(filename, "r");
  if (fp == NULL) {
    free(loader);
    free(surfaces);
    return NULL;
  }

  int i = 0;
  while (getline(&line, &len, fp) != -1) {
    line[strcspn(line, "\n")] = '\0';
    surfaces[i] = IMG_Load(line);
    if(surfaces[i] == NULL) {
      printf("Error loading asset %s", line);
    }
  }

  fclose(fp);
  if (line) {
    free(line);
  }

  loader->_assets = surfaces;
  loader->_assetCount = assetCount;

  return loader;
}

SDL_Surface* COIAssetLoaderGetAsset(COIAssetLoader* loader, int index) {
  return loader->_assets[index];
}

void COIAssetLoaderDestroy(COIAssetLoader* loader) {
  int i;
  for (i = 0; i < loader->_assetCount; i++) {
    if (loader->_assets[i] != NULL) {
      SDL_FreeSurface(loader->_assets[i]);
    }    
  }
  free(loader->_assets);
  free(loader);
}

