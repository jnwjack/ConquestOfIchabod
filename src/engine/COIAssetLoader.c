#include "COIAssetLoader.h"

COIAssetLoader* COI_GLOBAL_LOADER = NULL;

COIAssetLoader* COIAssetLoaderCreate() {
  FILE* fp = NULL;
  char* line = NULL;
  size_t len = 0;
  const char* mappingFilename = "src/engine/etc/mapping.cfg";
  const char* collisionsFilename = "src/engine/etc/collisions.cfg";
  const char* textFilename = "src/engine/etc/text.cfg";
  int assetCount = countLines(mappingFilename);

  printf("COIASSETLOADER\n");

  COIAssetLoader* loader = malloc(sizeof(COIAssetLoader));
  SDL_Surface** surfaces = malloc(assetCount * sizeof(SDL_Surface*));
  COIExtraCollision** collisions = malloc(assetCount * sizeof(COIExtraCollision*));

  int i;
  for (i = 0; i < assetCount; i++) {
    collisions[i] = NULL;
  }

  fp = fopen(mappingFilename, "r");
  printf("after fopen\n");
  if (fp == NULL) {
    printf("NULL!!!\n");
    free(loader);
    free(surfaces);
    free(collisions);
    return NULL;
  }

  i = 0;
  printf("LEN: %i\n", assetCount);
  while (getline(&line, &len, fp) != -1) {
    line[strcspn(line, "\n")] = '\0';
    printf("LINE: %s\n", line);
    surfaces[i] = IMG_Load(line);
    printf("loaded image\n");
    if(surfaces[i] == NULL) {
      printf("Error loading asset %s", line);
    }
    i++;
  }
  printf("loaded assets\n");
  
  if (line) {
    free(line);
  }
  fclose(fp);

  int assetID, tlX, tlY, brX, brY, ret;
  COIExtraCollision* collision;
  FILE* fpCol =  NULL;
  char* lineCol = NULL;
  len = 0;
  fpCol = fopen(collisionsFilename, "r");
  if (fpCol == NULL) {
    free(loader);
    free(surfaces);
    free(collisions);
    return NULL;
  }

    
  while (getline(&lineCol, &len, fpCol) != -1) {
    lineCol[strcspn(lineCol, "\n")] = '\0';

    
    assetID = atoi(strtok(lineCol, " "));
    tlX = atoi(strtok(NULL, " "));
    tlY = atoi(strtok(NULL, " "));
    brX = atoi(strtok(NULL, " "));
    brY = atoi(strtok(NULL, " "));
    ret = atoi(strtok(NULL, " "));
    
    if (assetID >= assetCount) {
      printf("Error loading collisions: asset ID is out of range.\n");
      continue;
    }

    collision = malloc(sizeof(COIExtraCollision));
    collision->tlX = tlX;
    collision->tlY = tlY;
    collision->brX = brX;
    collision->brY = brY;
    collision->returnValue = ret;
    
    collisions[assetID] = collision;
  }
  
  if (lineCol) {
    free(lineCol);
  }
  fclose(fpCol);

  loader->_assets = surfaces;
  loader->_assetCount = assetCount;
  loader->_extraCollisions = collisions;
  
  return loader;
}

SDL_Surface* COIAssetLoaderGetAsset(COIAssetLoader* loader, int index) {
  return loader->_assets[index];
}

COIExtraCollision* COIAssetLoaderGetCollision(COIAssetLoader* loader, int index) {
  return loader->_extraCollisions[index];
}

void COIAssetLoaderInit() {
  COI_GLOBAL_LOADER = COIAssetLoaderCreate();
}

void COIAssetLoaderDestroy(COIAssetLoader* loader) {
  int i;
  for (i = 0; i < loader->_assetCount; i++) {
    if (loader->_assets[i] != NULL) {
      SDL_FreeSurface(loader->_assets[i]);
    }
    if (loader->_extraCollisions[i] != NULL) {
      free(loader->_extraCollisions[i]);
    }
  }
  free(loader->_assets);
  free(loader->_extraCollisions);
  free(loader);
}

