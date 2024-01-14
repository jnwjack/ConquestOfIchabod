#ifndef COIMENU_H
#define COIMENU_H

#include "COISprite.h"
#include "COIString.h"
#include "COITextType.h"
#include "COIBoard.h"
#include <stdbool.h>
#include <stdlib.h>

#define COI_MENU_OFFSET_X 50
#define COI_MENU_OFFSET_Y 10

typedef struct COIMenu {
  COISprite* _frame;
  COISprite* _pointer;
  int _x;
  int _y;
  int _width;
  int _height;
  COIString** _strings;
  int _stringCount;
  int _visibleTextCount;
  int _current;
  int _fontSize;
  // Both frame bounds are inclusive
  int _lowerFrameBound;
  int _upperFrameBound;
}COIMenu;

COIMenu* COIMenuCreate(COISprite* frame, COISprite* pointer);
void COIMenuDestroy(COIMenu* menu);
void COIMenuDestroyAndFreeComponents(COIMenu* menu, COIBoard* board);

void COIMenuSetVisible(COIMenu* menu);
void COIMenuSetInvisible(COIMenu* menu);
void COIMenuSetTexts(COIMenu* menu, COIString* stringPointers[], int numIndices);
void COIMenuAdjustFrame(COIMenu* menu);
void COIMenuReset(COIMenu* menu);
void COIMenuIncrement(COIMenu* menu, int step);
bool COIMenuHandleInput(COIMenu* menu, int event);

#endif
