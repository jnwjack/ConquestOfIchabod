#ifndef COICHAR_H
#define COICHAR_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "COITextType.h"

typedef struct COIChar {
  SDL_Rect* drawRect;
  SDL_Texture* texture;
  
  int x;
  int y;
  int w;
  int h;
  bool visible;
  char value;
  struct COIChar* next;
}COIChar;

COIChar* COICharCreate(char c, int x, int y, COITextType* textType);
void COICharDestroy(COIChar* coiChar);

void COICharSetPos(COIChar* coiChar, int x, int y);


#endif
