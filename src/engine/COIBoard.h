#ifndef COIBOARD_H
#define COIBOARD_H

#define INDEX_RED   0
#define INDEX_GREEN 1
#define INDEX_BLUE  2
#define INDEX_ALPHA 3

#include <stdlib.h>
#include <SDL2/SDL.h>
#include "COISprite.h"

typedef struct COIBoard {
  int _bgColor[4];
  COISprite* _sprites;
  
}COIBoard;

COIBoard* COIBoardCreate(int r, int g, int b, int a);
void COIBoardDestroy(COIBoard* board);

int COIBoardBGColor(COIBoard* board, int index);

#endif
