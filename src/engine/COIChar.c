#include "COIChar.h"

COIChar* COICharCreate(char c, int x, int y, COITextType* textType) {
  COIChar* obj = malloc(sizeof(COIChar));

  obj->value = c;
  obj->x = x;
  obj->y = y;
  obj->drawRect = malloc(sizeof(SDL_Rect));
  obj->visible = true;
  obj->next = NULL;
  
  char str[2];
  str[0] = obj->value;
  str[1] = '\0';
  SDL_Surface* surface = TTF_RenderText_Solid(textType->font, (const char*)&str, textType->color);
  obj->texture = SDL_CreateTextureFromSurface(textType->renderer, surface);
  SDL_FreeSurface(surface);

  SDL_QueryTexture(obj->texture, NULL, NULL, &obj->w, &obj->h);
  obj->drawRect->x = x;
  obj->drawRect->y = y;
  obj->drawRect->w = obj->w;
  obj->drawRect->h = obj->h;

  return obj;
}

void COICharDestroy(COIChar* coiChar) {
  free(coiChar->drawRect);
  if (coiChar->next != NULL) {
    COICharDestroy(coiChar->next);
  }
  SDL_DestroyTexture(coiChar->texture);
  free(coiChar);
}

void COICharSetPos(COIChar* coiChar, int x, int y) {
  coiChar->x = x;
  coiChar->y = y;
  coiChar->drawRect->x = x;
  coiChar->drawRect->y = y;
}
