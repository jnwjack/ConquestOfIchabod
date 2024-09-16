#ifndef COISTRING_H
#define COISTRING_H

#include "COISprite.h"
#include "COITextType.h"
#ifdef __WINDOWS__
#define SDL_MAIN_HANDLED
#endif
#include <SDL2/SDL.h>
#include <stdbool.h>


#define MAX_STRING_SIZE 100
#define TYPING_TICKS_PER_CHAR 3

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


typedef struct COIString {
  COIChar* _head;
  COIChar* _typingAnimationHead; // Most recent visible COIChar
  int _typingAnimationTicks;
  
  int x;
  int y;
  int yBottomLine; // Y-coord of lowest line on string. Useful when confining to sprites
  int fontSize;
  bool visible;

  int index; // Used by COIBoard to identify position in COIString list
}COIString;

COIString* COIStringCreate(char* string, int x, int y, COITextType* textType);
void COIStringDestroy(COIString* obj);
void COIStringDraw(COIString* obj, SDL_Renderer* renderer);
void COIStringSetPos(COIString* obj, int x, int y);
void COIStringSetVisible(COIString* obj, bool visible);
void COIStringSetPosWithWrapping(COIString* obj, int x, int y, int width);
void COIStringConfineToSprite(COIString* obj, COISprite* sprite);
void COIStringPositionBelowString(COIString* below, COIString* above, bool onlySetY);
void COIStringPositionRightOfString(COIString* right, COIString* left, int space);
bool COIStringAnimateTyping(COIString* string);
void COIStringSetY(COIString* obj, int y);
COIString** COIStringCopyList(COIString** src, int size);
void COIStringSetStringCenter(COIString* string, int x, int y);

#endif
