#include "COISprite.h"
#include "COIAssetLoader.h"
#include "util.h"

#ifndef COIBOARD_H
#define COIBOARD_H

#define INDEX_RED   0
#define INDEX_GREEN 1
#define INDEX_BLUE  2
#define INDEX_ALPHA 3


typedef struct COIBoard {
  int _bgColor[4];
  struct COISprite** _sprites;
  int _spriteCount;
  
}COIBoard;

COIBoard* COIBoardCreate(int r, int g, int b, int a);
void COIBoardDestroy(COIBoard* board);

int COIBoardBGColor(COIBoard* board, int index);
void COIBoardLoadSpriteMap(COIBoard* board, struct COIAssetLoader* loader, struct SDL_Renderer* renderer, const char* filename);

#endif
