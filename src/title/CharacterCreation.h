#ifndef CHARACTERCREATION_H
#define CHARACTERCREATION_H

#include "../engine/COI.h"

#define KEYBOARD_GRID_WIDTH 10 // Not true for last row
#define KEYBOARD_GRID_HEIGHT 3
#define KEYBOARD_NUM_CHARS 30
#define KEYBOARD_OFFSET_X 100
#define KEYBOARD_OFFSET_Y 250
#define KEYBOARD_GRID_PADDING 16
#define KEYBOARD_NAME_SIZE 8
typedef struct Keyboard {
  char name[KEYBOARD_NAME_SIZE + 1];
  unsigned int currentNameChar;
  char characters[KEYBOARD_NUM_CHARS];
  COIString* gridStrings[KEYBOARD_NUM_CHARS];
  COIString* underscores[KEYBOARD_NAME_SIZE];
  COIString* nameStrings[KEYBOARD_NAME_SIZE];
  COISprite* highlight;
  COITextType* textType;
  int currentGridX;
  int currentGridY;
} Keyboard;

void KeyboardInit(Keyboard* kb, COIBoard* board);
void KeyboardMoveCursor(Keyboard* kb, int dX, int dY);
void KeyboardAddCharacter(Keyboard* kb, COIBoard* board);
void KeyboardRemoveCharacter(Keyboard* kb, COIBoard* board);

#endif