#ifndef TITLE_H
#define TITLE_H

#include "../engine/COI.h"

#define TITLE_NAME_TICKS 8

typedef struct TitleContext {
  COIMenu* menu;
  COISprite* name;
  COISprite* drawing;
  unsigned int nameTicks;
  COIBoard* board;
} TitleContext;

COIBoard* titleCreateBoard();
void titleDestroyBoard(TitleContext* context);
void titleTick(TitleContext* context);

#endif
