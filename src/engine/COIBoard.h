#include "COISprite.h"
#include "COIAssetLoader.h"
#include "util.h"
#include <stdbool.h>

#ifndef COIBOARD_H
#define COIBOARD_H

#define INDEX_RED   0
#define INDEX_GREEN 1
#define INDEX_BLUE  2
#define INDEX_ALPHA 3


typedef struct COIBoard {
  int _bgColor[4];
  int _width;
  int _height;
  int _frameX;
  int _frameY;
  COISprite** _sprites;
  int _spriteCount;
  
}COIBoard;

COIBoard* COIBoardCreate(int r, int g, int b, int a, int w, int h);
void COIBoardDestroy(COIBoard* board);

int COIBoardBGColor(COIBoard* board, int index);
void COIBoardLoadSpriteMap(COIBoard* board, struct COIAssetLoader* loader, SDL_Renderer* renderer, const char* filename);
COISprite** COIBoardGetSprites(COIBoard* board);
int COIBoardGetSpriteCount(COIBoard* board);
bool COIBoardShiftFrameX(COIBoard* board, int stride, int frameWidth);
bool COIBoardShiftFrameY(COIBoard* board, int stride, int frameHeight);
void COIBoardUpdateSpriteVisibility(COIBoard* board, int frameWidth, int frameHeight);


#endif
