#ifndef COIMENU_H
#define COIMENU_H

#include "COISprite.h"
#include "COITextGroup.h"
#include <stdbool.h>
#include <stdlib.h>

#define COI_MENU_OFFSET_X 100
#define COI_MENU_OFFSET_Y 10
#define COI_MENU_PADDING 10

typedef struct COIMenu {
  COISprite* _frame;
  COISprite* _pointer;
  COITextGroup* _group;
  int _x;
  int _y;
  int _width;
  int _height;
  COIText** _texts;
  int _textsCount;
  int _visibleTextCount;
  int _current;
  // Both frame bounds are inclusive
  int _lowerFrameBound;
  int _upperFrameBound;
}COIMenu;

COIMenu* COIMenuCreate(COITextGroup* group,
		       COISprite* frame,
		       COISprite* pointer);
void COIMenuDestroy(COIMenu* menu);

void COIMenuSetVisible(COIMenu* menu);
void COIMenuSetInvisible(COIMenu* menu);
void COIMenuSetTexts(COIMenu* menu, int indices[], int numIndices);
void COIMenuAdjustFrame(COIMenu* menu);
void COIMenuReset(COIMenu* menu);
void COIMenuIncrement(COIMenu* menu, int step);

#endif
