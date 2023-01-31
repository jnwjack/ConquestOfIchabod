#ifndef COISTRING_H
#define COISTRING_H

#include "COIChar.h"

typedef struct COIString {
  COIChar* _head;
  
  int x;
  int y;
  bool visible;
}COIString;

COIString* COIStringCreate(char* string, int x, int y, COITextType* textType);
void COIStringDestroy(COIString* obj);
void COIStringDraw(COIString* obj, SDL_Renderer* renderer);

#endif
