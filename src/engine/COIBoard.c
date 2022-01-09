#include "COIBoard.h"

COIBoard* COIBoardCreate(int r, int g, int b, int a) {
  COIBoard* board = malloc(sizeof(COIBoard));
  board->_bgColor[INDEX_RED] = r;
  board->_bgColor[INDEX_GREEN] = g;
  board->_bgColor[INDEX_BLUE] = b;
  board->_bgColor[INDEX_ALPHA] = a;
  board->_sprites = NULL;
  return board;
}

void COIBoardDestroy(COIBoard* board) {
  if (board->_sprites != NULL) {
    int i;
    for (i = 0; i < board->_spriteCount; i++) {
      free(board->_sprites[i]);
    }
    free(board->_sprites);
  }
  free(board);
}

int COIBoardBGColor(COIBoard* board, int index) {
  return board->_bgColor[index];
}

void COIBoardLoadSpriteMap(COIBoard* board, struct COIAssetLoader* loader,  struct SDL_Renderer* renderer, const char* filename) {
  FILE* fp;
  
  fp = fopen(filename, "r");
  if (fp == NULL) {
    return;
  }

  int spriteCount = countLines(filename);
  board->_sprites = malloc(spriteCount * sizeof(COISprite*));
  board->_spriteCount = spriteCount;

  size_t len = 0;
  char* line;
  int assetID;
  int x;
  int y;
  int i = 0;
  while (getline(&line, &len, fp) != -1) {
    line[strcspn(line, "\n")] = '\0';

    assetID = atoi(strtok(line, " "));
    x = atoi(strtok(NULL, " "));
    y = atoi(strtok(NULL, " "));

    printf("%i %i %i\n", assetID, x, y);
    SDL_Surface* asset = COIAssetLoaderGetAsset(loader, assetID);
    SDL_Texture* texture  = SDL_CreateTextureFromSurface(renderer, asset);
    board->_sprites[i] = COISpriteCreate(x, y, texture);
    
    i++;
  }
}
