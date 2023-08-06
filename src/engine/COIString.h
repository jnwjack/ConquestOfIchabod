#ifndef COISTRING_H
#define COISTRING_H

#include "COIChar.h"

#define MAX_STRING_SIZE 100

typedef struct COIString {
  COIChar* _head;
  
  int x;
  int y;
  int fontSize;
  bool visible;

  int index; // Used by COIBoard to identify position in COIString list
}COIString;

COIString* COIStringCreate(char* string, int x, int y, COITextType* textType);
void COIStringDestroy(COIString* obj);
void COIStringDraw(COIString* obj, SDL_Renderer* renderer);
void COIStringSetPos(COIString* obj, int x, int y);
void COIStringSetVisible(COIString* obj, bool visible);
COIString** COIStringCopyList(COIString** src, int size);

#endif
