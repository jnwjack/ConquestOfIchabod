#ifndef CHARACTERCREATION_H
#define CHARACTERCREATION_H

#include "../engine/COI.h"
#include "../player.h"

#define KEYBOARD_GRID_WIDTH 10 // Not true for last row
#define KEYBOARD_GRID_HEIGHT 3
#define KEYBOARD_NUM_CHARS 30
#define KEYBOARD_OFFSET_X 100
#define KEYBOARD_OFFSET_Y 300
#define KEYBOARD_GRID_PADDING 16
#define KEYBOARD_NAME_SIZE 8
typedef struct Keyboard {
  char name[KEYBOARD_NAME_SIZE + 1];
  unsigned int currentNameChar;
  char characters[KEYBOARD_NUM_CHARS];
  COIString* gridStrings[KEYBOARD_NUM_CHARS];
  COIString* underscores[KEYBOARD_NAME_SIZE];
  COIString* nameStrings[KEYBOARD_NAME_SIZE];
  COIString* end;
  COISprite* highlight;
  COITextType* textType;
  COITextType* textTypeWords;
  int currentGridX;
  int currentGridY;
} Keyboard;

void KeyboardInit(Keyboard* kb, COIBoard* board);
void KeyboardMoveCursor(Keyboard* kb, int dX, int dY);
bool KeyboardSelect(Keyboard* kb, COIBoard* board);
void KeyboardRemoveCharacter(Keyboard* kb, COIBoard* board);
void KeyboardSetVisible(Keyboard* kb, bool visible);
bool KeyboardIsVisible(Keyboard* kb);
void KeyboardDestroy(Keyboard* kb, COIBoard* board);

#define CLASSSELECTOR_NUM_CLASSES 3
#define CLASSSELECTOR_OFFSET_X 150
#define CLASSSELECTOR_OFFSET_Y 50
typedef struct ClassSelector {
  COISprite* leftArrow;
  COISprite* rightArrow;
  COISprite* classIcons[CLASSSELECTOR_NUM_CLASSES];
  COIString* classNames[CLASSSELECTOR_NUM_CLASSES];
  int currentClass;
} ClassSelector;

void ClassSelectorInit(ClassSelector* cs, COIBoard* board);
void ClassSelectorChange(ClassSelector* cs, int d);
void ClassSelectorSetVisible(ClassSelector* cs, bool visible);
bool ClassSelectorIsVisible(ClassSelector* cs);
void ClassSelectorDestroy(ClassSelector* cs, COIBoard* board);

#endif