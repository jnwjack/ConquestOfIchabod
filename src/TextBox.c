#include "TextBox.h"

TextBox* TextBoxCreate(COIBoard* board, COITextType* textType) {
  TextBox* textBox = malloc(sizeof(TextBox));
  textBox->board = board;
  textBox->textType = textType;
  textBox->strings = LinkedListCreate();
  textBox->currentString = NULL;
  textBox->currentStringDone = false;
  textBox->box = COISpriteCreateFromAssetID(70, 360, 500, 100,
					    COI_GLOBAL_LOADER,
					    5,
					    COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  textBox->box->_autoHandle = false;
  textBox->box->_visible = false;
  COIBoardAddDynamicSprite(textBox->board, textBox->box, 1);

  return textBox;
}

static void _destroyStrings(TextBox* textBox) {
  LinkedListResetCursor(textBox->strings);
  void* currentStringVoid = LinkedListNext(textBox->strings);
  COIString* currentString = (COIString*)currentStringVoid;
  while (currentString != NULL) {
    LinkedListRemove(textBox->strings, currentStringVoid);
    COIBoardRemoveString(textBox->board, currentString, 1);
    COIStringDestroy(currentString);
    
    currentStringVoid = LinkedListNext(textBox->strings);
    currentString = (COIString*)currentStringVoid;
  }
}

void TextBoxSetTicksPerChar(TextBox* textBox, int ticks) {
  LinkedListResetCursor(textBox->strings);
  COIString* current = (COIString*)LinkedListNext(textBox->strings);
  while (current) {
    current->ticksPerChar = ticks;
    current = (COIString*)LinkedListNext(textBox->strings);
  }
  LinkedListResetCursor(textBox->strings);
}

void TextBoxSetStrings(TextBox* textBox, char* firstString, ...) {
  // Clean up previous strings if we need to
  _destroyStrings(textBox);
  
  va_list list;
  char* currentRawString = firstString;
  va_start(list, firstString);
  while (currentRawString != NULL) {
    COIString* newString = COIStringCreate(currentRawString, 0, 0, textBox->textType);
    COIStringConfineToSprite(newString, textBox->box);
    COIBoardAddString(textBox->board, newString, 1);
    COIStringSetVisible(newString, false);
    LinkedListAdd(textBox->strings, (void*)newString);

    currentRawString = va_arg(list, char*);
  }
  va_end(list);
  LinkedListResetCursor(textBox->strings);
  textBox->currentString = (COIString*)LinkedListNext(textBox->strings);

  textBox->box->_visible = true;
  textBox->currentStringDone = false;
}

// Continue typing animation for current string.
void TextBoxAnimate(TextBox* textBox) {
  if (COIStringAnimateTyping(textBox->currentString)) {
    textBox->currentStringDone = true;
  }
}

// If the current string is done animating, move on to the next string.
// If it isn't, display the rest of the string. User is clicking through text.
// If there are no more strings, close the text box.
void TextBoxNextString(TextBox* textBox) {
  if (textBox->currentStringDone) {
    COIString* nextString = (COIString*)LinkedListNext(textBox->strings);
    COIStringSetVisible(textBox->currentString, false);
    textBox->currentString = nextString;
    textBox->currentStringDone = false;
    if (textBox->currentString == NULL) {
      textBox->box->_visible = false;
    }
  } else {
    COIStringSetVisible(textBox->currentString, true);
    textBox->currentStringDone = true;
  }
}

void TextBoxDestroy(TextBox* textBox) {
  _destroyStrings(textBox);
  LinkedListDestroy(textBox->strings);
  COIBoardRemoveDynamicSprite(textBox->board, textBox->box, 0);
  COISpriteDestroy(textBox->box);
  free(textBox);
}
