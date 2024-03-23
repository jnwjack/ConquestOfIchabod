#ifndef TITLE_H
#define TITLE_H

#include "../engine/COI.h"
#include "../TextBox.h"

//#define TITLE_NAME_TICKS 24
#define TITLE_NAME_TICKS 12
#define TITLE_TEXT_ANIMATE_TICKS 0 // Reduce text speed by this factor
#define TITLE_STRING_NEW_GAME 0
#define TITLE_STRING_CONTINUE_GAME 1
#define TITLE_STRING_QUIT_GAME 2

#define TITLE_NUM_INTRO_SLIDES 5
#define TITLE_NUM_OPTIONS 3

typedef enum TitleNextBoard {
  TITLE_NEW_GAME,
  TITLE_CONTINUE_GAME,
  TITLE_TITLE
} TitleNextBoard;

typedef struct TitleContext {
  COIMenu* menu;
  COISprite* name;
  COISprite* drawing;
  COISprite* slides[TITLE_NUM_INTRO_SLIDES];
  unsigned int ticks;
  bool animating; // Title animation
  char currentSlide; // -1 when on title
  COIBoard* board;
  COIString* strings[TITLE_NUM_OPTIONS]; // Selected option
  COIString* grayStrings[TITLE_NUM_OPTIONS]; // Unselected option
  int selectedStringIndex;
  TextBox* textBox;
  COITextType* tBoxTextType;
} TitleContext;

COIBoard* titleCreateBoard();
void titleDestroyBoard(TitleContext* context);
void titleTick(TitleContext* context);
void titleProcessInput(TitleContext* context, int direction);
TitleNextBoard titleGetNextBoard(TitleContext* context);

#endif
