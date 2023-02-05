#include "COIString.h"

COIString* COIStringCreate(char* string, int x, int y, COITextType* textType) {
  COIString* obj = malloc(sizeof(COIString));

  obj->fontSize = textType->fontSize;

  // Store string as a linked list of COIChars
  COIChar* current = NULL;
  COIChar* prev = NULL;
  int offsetX = x, offsetY = y;
  int i = 0;
  char c = string[i];
  while (c != '\0') {
    current = COICharCreate(c, offsetX, offsetY, textType);
    if (prev != NULL) {
      prev->next = current;
    } else {
      obj->_head = current;
    }

    // Set offsets to where we want to draw the next COIChar
    offsetX += current->w;

    prev = current;
    i++;
    c = string[i];
  }

  return obj;
}

void COIStringDestroy(COIString* obj) {
  COICharDestroy(obj->_head);
  free(obj);
}

void COIStringDraw(COIString* obj, SDL_Renderer* renderer) {
  COIChar* current = obj->_head;
  while (current != NULL) {
    if (current->visible) {
      SDL_RenderCopy(renderer, current->texture, NULL, current->drawRect);
    }
    current = current->next;
  }
}

void COIStringSetPos(COIString* obj, int x, int y) {
  COIChar* current = obj->_head;
  int offsetX = x;
  while (current != NULL) {
    COICharSetPos(current, offsetX, y);
    offsetX += current->w;
    current = current->next;
  }
}

void COIStringSetVisible(COIString* obj, bool visible) {
  COIChar* current = obj->_head;
  while (current != NULL) {
    current->visible = visible;
    current = current->next;
  }
}
