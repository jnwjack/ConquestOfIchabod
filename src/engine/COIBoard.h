#ifndef COIBOARD_H
#define COIBOARD_H

#include "COIString.h"
#include "COISprite.h"
#include "COIAssetLoader.h"
#include "util.h"
#include "../TimeState.h"
#include <stdbool.h>

#define INDEX_RED   0
#define INDEX_GREEN 1
#define INDEX_BLUE  2
#define INDEX_ALPHA 3

#define COIBOARD_MAX_STRINGS 100
#define COIBOARD_MAX_RECTS 50
#define COIBOARD_NUM_DRAW_LAYERS 2

typedef struct COIRect {
  SDL_Rect sdlRect;
  bool visible;
  Uint8 r;
  Uint8 g;
  Uint8 b;
  Uint8 a;
} COIRect;

typedef struct Layer {
  LinkedList* dynamicSprites;
  COIString** strings;
  int stringCount;
  COIRect rects[COIBOARD_MAX_RECTS];
  int numRects;
} Layer;


typedef struct COIBoard {
  int _bgColor[4];
  int _width;
  int _height;
  
  // View window dimensions
  int _frameX;
  int _frameY;
  int _frameWidth;
  int _frameHeight;

  Layer drawLayers[COIBOARD_NUM_DRAW_LAYERS];

  // Sprites loaded in via spritemap
  // New structure for static sprites
  COISprite** spriteGrid;
  int spriteGridHeight;
  int spriteGridWidth;
  
  // Sprites loaded in dynamically that persist after COIBoard desturction
  // It'd be better to structure the code so we don't need this
  COISprite** persistentSprites;
  int perSpriteCount;
  
  int _shouldDraw;
  void* context;

  // Used to load spritemap
  COIAssetLoader* loader;
}COIBoard;

COIBoard* COIBoardCreate(int r, int g, int b, int a, int w, int h, COIAssetLoader* loader);
void COIBoardDestroy(COIBoard* board);

int COIBoardBGColor(COIBoard* board, int index);
void COIBoardLoadSpriteMap(COIBoard* board, SDL_Renderer* renderer, const char* filename);
void COIBoardSetPersistentSprites(COIBoard* board, COISprite** sprites, int count);
bool COIBoardShiftFrameX(COIBoard* board, int stride);
bool COIBoardShiftFrameY(COIBoard* board, int stride);
void COIBoardUpdateSpriteVisibility(COIBoard* board);
void COIBoardMoveSprite(COIBoard* board, COISprite* sprite, int x, int y);
void COIBoardQueueDraw(COIBoard* board); // Request draw for this board
void COIBoardSetContext(COIBoard* board, void* context);
// void COIBoardSetStrings(COIBoard* board, COIString** strings, int count);
bool COIBoardAddString(COIBoard* board, COIString* string, unsigned int layer);
bool COIBoardAddSprite(COIBoard* board, COISprite* sprite);
void COIBoardRemoveString(COIBoard* board, COIString* string, unsigned int layer);
void COIBoardAddDynamicSprite(COIBoard* board, COISprite* sprite, unsigned int layer);
void COIBoardRemoveDynamicSprite(COIBoard* board, COISprite* sprite, unsigned int layer);
void COIBoardAdjustSprite(COIBoard* board, COISprite* sprite);
void COIBoardUpdateBGColor(COIBoard* board);
void COIBoardAddRect(COIBoard* board, int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool visible, unsigned int layer);

#endif
