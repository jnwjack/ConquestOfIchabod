#ifndef COITEXTGROUP_H
#define COITEXTGROUP_H

#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include "util.h"

typedef struct COITextType {
  TTF_Font* font;
  SDL_Color color;
  int fontSize;
  SDL_Renderer* renderer;
}COITextType;

COITextType* COITextTypeCreate(int fontSize, int r, int g, int b, SDL_Renderer* renderer);
void COITextTypeDestroy(COITextType* type);

#endif
