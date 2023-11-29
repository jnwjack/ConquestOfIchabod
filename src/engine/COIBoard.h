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

#define COIBOARD_MAX_STRINGS 100
#define COIBOARD_GRID_SIZE 32


typedef struct COIBoard {
  int _bgColor[4];
  int _width;
  int _height;
  
  // View window dimensions
  int _frameX;
  int _frameY;
  int _frameWidth;
  int _frameHeight;

  // Sprites loaded in via spritemap
  COISprite** _sprites;
  // Sprites loaded in "on-the-fly"
  COISprite** dynamicSprites;
  int _spriteCount;
  int dynSpriteCount;
  
  int _shouldDraw;
  void* context;

  // New method of handling strings
  COIString** strings;
  int stringCount;

  // Used to load spritemap
  COIAssetLoader* loader;
}COIBoard;

COIBoard* COIBoardCreate(int r, int g, int b, int a, int w, int h, COIAssetLoader* loader);
void COIBoardDestroy(COIBoard* board);

int COIBoardBGColor(COIBoard* board, int index);
void COIBoardLoadSpriteMap(COIBoard* board, SDL_Renderer* renderer, const char* filename);
void COIBoardSetDynamicSprites(COIBoard* board, COISprite** sprites, int count);
COISprite** COIBoardGetSprites(COIBoard* board);
int COIBoardGetSpriteCount(COIBoard* board);
bool COIBoardShiftFrameX(COIBoard* board, int stride);
bool COIBoardShiftFrameY(COIBoard* board, int stride);
void COIBoardUpdateSpriteVisibility(COIBoard* board);
void COIBoardMoveSprite(COIBoard* board, COISprite* sprite, int x, int y);
void COIBoardQueueDraw(COIBoard* board); // Request draw for this board
void COIBoardSetContext(COIBoard* board, void* context);
void COIBoardSetStrings(COIBoard* board, COIString** strings, int count);
bool COIBoardAddString(COIBoard* board, COIString* string);
void COIBoardRemoveString(COIBoard* board, COIString* string);

#endif
