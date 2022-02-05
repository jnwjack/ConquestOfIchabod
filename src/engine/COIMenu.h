#include "COISprite.h"
#include "COITextGroup.h"
#include <stdbool.h>
#include <stdlib.h>

#ifndef COIMENU_H
#define COIMENU_H

#define COI_MENU_OFFSET_X 60
#define COI_MENU_OFFSET_Y 10
#define COI_MENU_PADDING 20

typedef struct COIMenu {
  COISprite* _frame;
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
		       COISprite* frame);
void COIMenuDestroy(COIMenu* menu);

void COIMenuSetVisible(COIMenu* menu);
void COIMenuSetInvisible(COIMenu* menu);
void COIMenuSetTexts(COIMenu* menu, int indices[], int numIndices);
void COIMenuIncrement(COIMenu* menu, int step);

#endif
