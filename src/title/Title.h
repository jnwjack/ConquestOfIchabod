#ifndef TITLE_H
#define TITLE_H

#include "../engine/COI.h"

#define TITLE_NAME_TICKS 24
#define TITLE_STRING_NEW_GAME 0
#define TITLE_STRING_CONTINUE_GAME 1
#define TITLE_STRING_QUIT_GAME 2

typedef struct TitleContext {
  COIMenu* menu;
  COISprite* name;
  COISprite* drawing;
  unsigned int nameTicks;
  bool animating;
  COIBoard* board;
  COIString* strings[3]; // Selected option
  COIString* grayStrings[3]; // Unselected option
  int selectedStringIndex;
} TitleContext;

COIBoard* titleCreateBoard();
void titleDestroyBoard(TitleContext* context);
void titleTick(TitleContext* context);
void titleProcessInput(TitleContext* context, int direction);

#endif
