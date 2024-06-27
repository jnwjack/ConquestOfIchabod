#include "CharacterCreation.h"

void KeyboardInit(Keyboard* kb, COIBoard* board) {
  kb->textType = COITextTypeCreate(32, 255, 255, 255, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  kb->textTypeWords = COITextTypeCreate(20, 255, 255, 255, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  kb->currentNameChar = 0;

  for (char i = 0; i < 26; i++) {
    kb->characters[i] = 'A' + i; // Start at ASCII code for 'A' and add all letters
  }
  kb->characters[26] = '.';
  kb->characters[27] = ',';
  kb->characters[28] = '?';
  kb->characters[29] = '!';

  char buf[2];

  unsigned int row = 0;
  unsigned int col = 0;
  unsigned int maxCol = KEYBOARD_GRID_WIDTH - 2;
  for (int i = 0; i < KEYBOARD_NUM_CHARS; i++) {
    int x = KEYBOARD_OFFSET_X + (col * (kb->textType->fontSize + KEYBOARD_GRID_PADDING));
    int y = KEYBOARD_OFFSET_Y + (row * (kb->textType->fontSize + KEYBOARD_GRID_PADDING));
    snprintf(buf, 2, "%c", kb->characters[i]);
    kb->gridStrings[i] = COIStringCreate(buf, x, y, kb->textType);
    COIBoardAddString(board, kb->gridStrings[i]);
    if (col > maxCol) {
      row++;
      col = 0;
    } else {
      col++;
    }
  }
  kb->end = COIStringCreate("END",
                            KEYBOARD_OFFSET_X - 10,
                            KEYBOARD_OFFSET_Y + ((KEYBOARD_GRID_HEIGHT) * (kb->textType->fontSize + KEYBOARD_GRID_PADDING)),
                            kb->textTypeWords);
  COIBoardAddString(board, kb->end);

  const int underscoreY = KEYBOARD_OFFSET_Y - (2 * (kb->textType->fontSize + KEYBOARD_GRID_PADDING));
  for (int i = 0; i < KEYBOARD_NAME_SIZE; i++) {
    kb->name[i] = '\0'; // Initialize each char in name to \0
    int x = KEYBOARD_OFFSET_X + (i * (kb->textType->fontSize + KEYBOARD_GRID_PADDING));
    kb->underscores[i] = COIStringCreate("_", x, underscoreY, kb->textType);
    COIBoardAddString(board, kb->underscores[i]);
    kb->nameStrings[i] = NULL;
  }
  kb->name[KEYBOARD_NAME_SIZE] = '\0';

  kb->currentGridX = 0;
  kb->currentGridY = 0;

  kb->highlight = COISpriteCreateFromAssetID(-(KEYBOARD_GRID_PADDING) + KEYBOARD_OFFSET_X, 
                                            -(KEYBOARD_GRID_PADDING / 2) + KEYBOARD_OFFSET_Y, 
                                            kb->textType->fontSize + KEYBOARD_GRID_PADDING, 
                                            kb->textType->fontSize + KEYBOARD_GRID_PADDING,
                                            COI_GLOBAL_LOADER, 45, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  kb->highlight->_autoHandle = false;                                            
  COIBoardAddDynamicSprite(board, kb->highlight);
}

void KeyboardMoveCursor(Keyboard* kb, int dX, int dY) {
  kb->currentGridY += dY;

  if (kb->currentGridY < 0) {
    kb->currentGridY = KEYBOARD_GRID_HEIGHT;
    kb->currentGridX = 0;
  } else if (kb->currentGridY > KEYBOARD_GRID_HEIGHT) {
    kb->currentGridY = 0;
    kb->currentGridX = 0;
  }

  if (kb->currentGridY != KEYBOARD_GRID_HEIGHT) {
    kb->currentGridX += dX;
    if (kb->currentGridX < 0) {
      kb->currentGridX = KEYBOARD_GRID_WIDTH - 1;
    } else if (kb->currentGridX >= KEYBOARD_GRID_WIDTH) {
      kb->currentGridX = 0;
    }
  } else {
    kb->currentGridX = 0;
  }

  int newX = -(KEYBOARD_GRID_PADDING) + KEYBOARD_OFFSET_X + kb->currentGridX * (kb->textType->fontSize + KEYBOARD_GRID_PADDING);
  int newY = -(KEYBOARD_GRID_PADDING / 2) + KEYBOARD_OFFSET_Y + kb->currentGridY * (kb->textType->fontSize + KEYBOARD_GRID_PADDING);
  COISpriteSetPos(kb->highlight, newX, newY);
}

// Returns true when we're selecting 'END'
bool KeyboardSelect(Keyboard* kb, COIBoard* board) {
  const int underscoreY = KEYBOARD_OFFSET_Y - (2 * (kb->textType->fontSize + KEYBOARD_GRID_PADDING));
  const int newCharX = KEYBOARD_OFFSET_X + (kb->currentNameChar * (kb->textType->fontSize + KEYBOARD_GRID_PADDING));
  char buf[2];
  if (kb->currentNameChar < KEYBOARD_NAME_SIZE) {
    unsigned int index = kb->currentGridX + (kb->currentGridY * KEYBOARD_GRID_WIDTH);
    char c = kb->characters[index];
    snprintf(buf, 2, "%c", c);
    kb->nameStrings[kb->currentNameChar] = COIStringCreate(buf, newCharX, underscoreY, kb->textType);
    COIBoardAddString(board, kb->nameStrings[kb->currentNameChar]);
    kb->currentNameChar++;
  }

  return kb->currentGridY == KEYBOARD_GRID_HEIGHT; // True if we're on 'END'
}

void KeyboardRemoveCharacter(Keyboard* kb, COIBoard* board) {
  if (kb->currentNameChar > 0) {
    COIBoardRemoveString(board, kb->nameStrings[kb->currentNameChar - 1]);
    COIStringDestroy(kb->nameStrings[kb->currentNameChar - 1]);
    kb->currentNameChar--;
  }
}

void KeyboardSetVisible(Keyboard* kb, bool visible) {
  for (int i = 0; i < KEYBOARD_NUM_CHARS; i++) {
    COIStringSetVisible(kb->gridStrings[i], visible);
  }
  for (int i = 0; i < kb->currentNameChar; i++) {
    COIStringSetVisible(kb->nameStrings[i], visible);
  }
  for (int i = 0; i < KEYBOARD_NAME_SIZE; i++) {
    COIStringSetVisible(kb->underscores[i], visible);
  }
  COIStringSetVisible(kb->end, visible);
  kb->highlight->_visible = visible;
}

bool KeyboardIsVisible(Keyboard* kb) {
  return kb->highlight->_visible;
}

void KeyboardDestroy(Keyboard* kb, COIBoard* board) {
  COITextTypeDestroy(kb->textType);
  COITextTypeDestroy(kb->textTypeWords);
  COIBoardRemoveString(board, kb->end);
  COIStringDestroy(kb->end);
  COIBoardRemoveDynamicSprite(board, kb->highlight);
  COISpriteDestroy(kb->highlight);

  for (int i = 0; i < KEYBOARD_NUM_CHARS; i++) {
    COIBoardRemoveString(board, kb->gridStrings[i]);
    COIStringDestroy(kb->gridStrings[i]);
  }

  for (int i = 0; i < KEYBOARD_NAME_SIZE; i++) {
    COIBoardRemoveString(board, kb->underscores[i]);
    COIStringDestroy(kb->underscores[i]);
  }

  for (int i = 0; i < kb->currentNameChar; i++) {
    COIBoardRemoveString(board, kb->nameStrings[i]);
    COIStringDestroy(kb->nameStrings[i]);
  }
}

void ClassSelectorInit(ClassSelector* cs, COIBoard* board) {
  cs->leftArrow = COISpriteCreateFromAssetID(CLASSSELECTOR_OFFSET_X, CLASSSELECTOR_OFFSET_Y, 64, 64, 
                                            COI_GLOBAL_LOADER, 46, 
                                            COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COIBoardAddDynamicSprite(board, cs->leftArrow);
  cs->rightArrow = COISpriteCreateFromAssetID(CLASSSELECTOR_OFFSET_X + 240, CLASSSELECTOR_OFFSET_Y, 64, 64, 
                                            COI_GLOBAL_LOADER, 47, 
                                            COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COIBoardAddDynamicSprite(board, cs->rightArrow);

  COITextType* textType = COITextTypeCreate(32, 255, 255, 255, COIWindowGetRenderer(COI_GLOBAL_WINDOW));

  cs->currentClass = PLAYER_CLASS_FIGHTER;
  cs->classIcons[PLAYER_CLASS_FIGHTER] = COISpriteCreateFromAssetID(CLASSSELECTOR_OFFSET_X + 120, CLASSSELECTOR_OFFSET_Y, 64, 64,
                                                                    COI_GLOBAL_LOADER, 48,
                                                                    COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  cs->classIcons[PLAYER_CLASS_WIZARD] = COISpriteCreateFromAssetID(CLASSSELECTOR_OFFSET_X + 120, CLASSSELECTOR_OFFSET_Y, 64, 64,
                                                                   COI_GLOBAL_LOADER, 49,
                                                                   COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  cs->classIcons[PLAYER_CLASS_ROGUE] = COISpriteCreateFromAssetID(CLASSSELECTOR_OFFSET_X + 120, CLASSSELECTOR_OFFSET_Y, 64, 64,
                                                                  COI_GLOBAL_LOADER, 50,
                                                                  COIWindowGetRenderer(COI_GLOBAL_WINDOW));

  for (int i = 0; i < CLASSSELECTOR_NUM_CLASSES; i++) {
    bool visible = i == cs->currentClass;
    COIBoardAddDynamicSprite(board, cs->classIcons[i]);
    cs->classIcons[i]->_autoHandle = false;
    cs->classIcons[i]->_visible = visible;
    cs->classNames[i] = COIStringCreate(playerClassNameFromID(i), CLASSSELECTOR_OFFSET_X + 100, CLASSSELECTOR_OFFSET_Y + 100, textType);
    COIBoardAddString(board, cs->classNames[i]);
    COIStringSetVisible(cs->classNames[i], visible);
  }

  COITextTypeDestroy(textType);
}

void ClassSelectorChange(ClassSelector* cs, int d) {
  cs->classIcons[cs->currentClass]->_visible = false;
  COIStringSetVisible(cs->classNames[cs->currentClass], false);

  cs->currentClass += d;
  if (cs->currentClass < 0) {
    cs->currentClass = CLASSSELECTOR_NUM_CLASSES - 1;
  } else if (cs->currentClass >= CLASSSELECTOR_NUM_CLASSES) {
    cs->currentClass = 0;
  }

  cs->classIcons[cs->currentClass]->_visible = true;
  COIStringSetVisible(cs->classNames[cs->currentClass], true);
}

void ClassSelectorSetVisible(ClassSelector* cs, bool visible) {
  cs->leftArrow->_visible = visible;
  cs->rightArrow->_visible = visible;
  cs->classIcons[cs->currentClass]->_visible = visible;
  COIStringSetVisible(cs->classNames[cs->currentClass], visible);
}

bool ClassSelectorIsVisible(ClassSelector* cs) {
  return cs->leftArrow->_visible;
}

void ClassSelectorDestroy(ClassSelector* cs, COIBoard* board) {
  COIBoardRemoveDynamicSprite(board, cs->leftArrow);
  COISpriteDestroy(cs->leftArrow);
  COIBoardRemoveDynamicSprite(board, cs->rightArrow);
  COISpriteDestroy(cs->leftArrow);

  for (int i = 0; i < CLASSSELECTOR_NUM_CLASSES; i++) {
    COIBoardRemoveDynamicSprite(board, cs->classIcons[i]);
    COISpriteDestroy(cs->classIcons[i]);
    COIBoardRemoveString(board, cs->classNames[i]);
    COIStringDestroy(cs->classNames[i]);
  }
}

