#include "COIChar.h"

COIChar* COICharCreate(char c, int x, int y, COITextType* textType) {
  COIChar* obj = malloc(sizeof(COIChar*));

  obj->value = c;
  obj->x = x;
  obj->y = y;
  obj->_drawRect = malloc(sizeof(SDL_Rect));
  obj->visible = false;
  
  char str[2];
  str[0] = obj->value;
  str[1] = '\0';
  SDL_Surface* surface = TTF_RenderText_Solid(textType->font, &str, textType->color);
  obj->_texture = SLD_CreateTextureFromSurface(textType->renderer, surface);
  int w, h;
  SDL_QueryTexture(obj->_texture, NULL, NULL, &w, &h);
  obj->_drawRect->x = x;
  obj->_drawRect->y = y;
  obj->_drawRect->w = w;
  obj->_drawRect->h = h;

  return obj;
}

void COICharDestroy(COIChar* coiChar) {
  free(coiChar->_drawRect);
  SDL_DestroyTexture(coiChar->_texture);
  free(coiChar);
}
