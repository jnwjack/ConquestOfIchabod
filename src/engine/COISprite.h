#include <SDL2/SDL.h>

#ifndef COISPRITE_H
#define COISPRITE_H

typedef struct COISprite {
  int _x;
  int _y;
  struct SDL_Texture* _texture;
}COISprite;

COISprite* COISpriteCreate(int x, int y, struct SDL_Texture* texture);
void COISpriteDestroy(COISprite* sprite);

#endif
