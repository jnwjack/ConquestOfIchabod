#ifndef TEXTBOX_H
#define TEXTBOX_H

#include "../engine/COIWindow.h"

typedef struct TextBox {
  LinkedList* strings;
  COISprite* box;
  COIBoard* board;
  COITextType* textType;
} TextBox;

TextBox* TextBoxCreate(COIBoard* board, COITextType* textType);
void TextBoxSetStrings(TextBox* textBox, char* firstString, ...);
void TextBoxDestroy(TextBox* textBox);

#endif
