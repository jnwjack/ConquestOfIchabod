#ifndef TEXTBOX_H
#define TEXTBOX_H

/*#include "engine/COIWindow.h"*/
#include "engine/COI.h"

typedef struct TextBox {
  LinkedList* strings;
  COISprite* box;
  COIBoard* board;
  COITextType* textType;
  COIString* currentString;
  bool currentStringDone;
} TextBox;

TextBox* TextBoxCreate(COIBoard* board, COITextType* textType);
void TextBoxSetStrings(TextBox* textBox, char* firstString, ...);
void TextBoxAnimate(TextBox* textBox);
void TextBoxNextString(TextBox* textBox);
void TextBoxDestroy(TextBox* textBox);

#endif
