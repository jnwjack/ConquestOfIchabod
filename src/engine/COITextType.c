#include "COITextType.h"

COITextType* COITextTypeCreate(int fontSize, int r, int g, int b, SDL_Renderer* renderer) {
  COITextType* type = malloc(sizeof(COITextType));
  type->renderer = renderer;
  type->font = TTF_OpenFont("src/engine/etc/font.ttf", fontSize);
  if (type->font == NULL) {
    printf("Error loading font.\n");
  }

  type->color.r = r;
  type->color.g = g;
  type->color.b = b;
  type->color.a = 255;
  type->fontSize = fontSize;
  
  return type;
}

void COITextTypeSetColor(COITextType* type, int r, int g, int b) {
  type->color.r = r;
  type->color.g = g;
  type->color.b = b;
}

void COITextTypeDestroy(COITextType* type) {
  if (type == NULL) {
    return;
  }

  TTF_CloseFont(type->font);
  free(type);
}
