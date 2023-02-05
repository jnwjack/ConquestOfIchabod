#include "COIString.h"
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
  
  // View window dimensions
  int _frameX;
  int _frameY;
  int _frameWidth;
  int _frameHeight;
  
  COISprite** _sprites;
  int _spriteCount;
  int _shouldDraw;
  void* context;

  // New method of handling strings
  COIString** strings;
  int stringCount;
}COIBoard;

COIBoard* COIBoardCreate(int r, int g, int b, int a, int w, int h);
void COIBoardDestroy(COIBoard* board);

int COIBoardBGColor(COIBoard* board, int index);
void COIBoardLoadSpriteMap(COIBoard* board, COIAssetLoader* loader, SDL_Renderer* renderer, const char* filename);
COISprite** COIBoardGetSprites(COIBoard* board);
int COIBoardGetSpriteCount(COIBoard* board);
bool COIBoardShiftFrameX(COIBoard* board, int stride);
bool COIBoardShiftFrameY(COIBoard* board, int stride);
void COIBoardUpdateSpriteVisibility(COIBoard* board);
void COIBoardMoveSprite(COIBoard* board, COISprite* sprite, int x, int y);
void COIBoardQueueDraw(COIBoard* board); // Request draw for this board
void COIBoardSetContext(COIBoard* board, void* context);
void COIBoardSetStrings(COIBoard* board, COIString** strings, int count);

#endif
