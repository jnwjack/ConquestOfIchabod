#include "TextBox.h"

TextBox* TextBoxCreate(COIBoard* board, COITextType* textType) {
  TextBox* textBox = malloc(sizeof(TextBox));
  textBox->board = board;
  textBox->textType = textType;
  textBox->strings = LinkedListCreate();
  textBox->box = COISpriteCreateFromAssetID(70, 360, 500, 100,
					    COI_GLOBAL_LOADER,
					    5,
					    COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  textBox->box->_autoHandle = false;
  textBox->box->_visible = false;
  COIBoardAddDynamicSprite(textBox->board, textBox->box);

  return textBox;
}

static void _destroyStrings(TextBox* textBox) {
  LinkedListResetCursor(textBox->strings);
  void* currentStringVoid = LinkedListNext(textBox->strings);
  COIString* currentString = (COIString*)currentStringVoid;
  while (currentString != NULL) {
    LinkedListRemove(textBox->strings, currentStringVoid);
    COIBoardRemoveString(textBox->board, currentString);
    COIStringDestroy(currentString);
  }
}

void TextBoxSetStrings(TextBox* textBox, char* firstString, ...) {
  va_list list;
  char* currentRawString = firstString;
  va_start(list, firstString);
  while (currentRawString) {
    COIString* newString = COIStringCreate(currentRawString, 0, 0, textBox->textType);
    COIStringConfineToSprite(newString, textBox->box);
    COIBoardAddString(textBox->board, newString);
    COIStringSetVisible(newString, false);
    LinkedListAdd(textBox->strings, (void*)newString);

    currentRawString = va_arg(list, char*);
  }
  va_end(list);
}

void TextBoxDestroy(TextBox* textBox) {
  _destroyStrings(textBox);
  LinkedListDestroy(textBox->strings);
  COIBoardRemoveDynamicSprite(textBox->board, textBox->box);
  COISpriteDestroy(textBox->box);
  free(textBox);
}
