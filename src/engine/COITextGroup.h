#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include "util.h"

#ifndef COITEXTGROUP_H
#define COITEXTGROUP_H

typedef struct COIText {
  int _x;
  int _y;
  int _width;
  int _height;
  SDL_Rect* _drawRect;
  SDL_Texture* _texture;
  char* _string;
  bool _visible;
}COIText;

typedef struct COITextGroup {
  TTF_Font* _font;
  SDL_Color _color;
  int _textCount;
  int _fontSize;
  COIText** _texts;
}COITextGroup;

COITextGroup* COITextGroupCreate(int fontSize, int r, int g, int b, const char* filename, SDL_Renderer* renderer);
void COITextGroupDestroy(COITextGroup* group);

COIText** COITextGroupGetTexts(COITextGroup* group);
int COITextGroupGetTextCount(COITextGroup* group);
void COITextSetPos(COIText* text, int x, int y);

#endif
