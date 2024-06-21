#include "CharacterCreation.h"

void KeyboardInit(Keyboard* kb, COIBoard* board) {
  kb->textType = COITextTypeCreate(32, 255, 255, 255, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
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
  COIBoardAddDynamicSprite(board, kb->highlight);
}

void KeyboardMoveCursor(Keyboard* kb, int dX, int dY) {
  kb->currentGridX += dX;
  kb->currentGridY += dY;

  if (kb->currentGridX < 0) {
    kb->currentGridX = KEYBOARD_GRID_WIDTH - 1;
  } else if (kb->currentGridX >= KEYBOARD_GRID_WIDTH) {
    kb->currentGridX = 0;
  }

  if (kb->currentGridY < 0) {
    kb->currentGridY = KEYBOARD_GRID_HEIGHT - 1;
  } else if (kb->currentGridY >= KEYBOARD_GRID_HEIGHT) {
    kb->currentGridY = 0;
  }

  int newX = -(KEYBOARD_GRID_PADDING) + KEYBOARD_OFFSET_X + kb->currentGridX * (kb->textType->fontSize + KEYBOARD_GRID_PADDING);
  int newY = -(KEYBOARD_GRID_PADDING / 2) + KEYBOARD_OFFSET_Y + kb->currentGridY * (kb->textType->fontSize + KEYBOARD_GRID_PADDING);
  COISpriteSetPos(kb->highlight, newX, newY);
}

void KeyboardAddCharacter(Keyboard* kb, COIBoard* board) {
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
    // if (kb->nameStrings[kb->currentNameChar]) {
    //   COIBoardRemoveString(board, kb->nameStrings[i]);
    // }
  }
}

void KeyboardRemoveCharacter(Keyboard* kb, COIBoard* board) {
  if (kb->currentNameChar > 0) {
    COIBoardRemoveString(board, kb->nameStrings[kb->currentNameChar - 1]);
    COIStringDestroy(kb->nameStrings[kb->currentNameChar - 1]);
    kb->currentNameChar--;
  }
}