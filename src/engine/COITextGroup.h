#ifndef COITEXTGROUP_H
#define COITEXTGROUP_H

#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include "util.h"

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

typedef struct COITextType {
  TTF_Font* font;
  SDL_Color color;
  int fontSize;
  SDL_Renderer* renderer;
}COITextType;

COITextType* COITextTypeCreate(int fontSize, int r, int g, int b, SDL_Renderer* renderer);
COITextGroup* COITextGroupCreate(int fontSize, int r, int g, int b, const char* filename, SDL_Renderer* renderer);
void COITextTypeDestroy(COITextType* type);
void COITextGroupDestroy(COITextGroup* group);

COIText** COITextGroupGetTexts(COITextGroup* group);
int COITextGroupGetTextCount(COITextGroup* group);
void COITextSetPos(COIText* text, int x, int y);

#endif
