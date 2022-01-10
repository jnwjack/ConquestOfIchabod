#include <SDL2/SDL.h>
#include <stdbool.h>

#ifndef COISPRITE_H
#define COISPRITE_H

typedef struct COISprite {
  int _x;
  int _y;
  int _width;
  int _height;
  bool _visible;
  SDL_Rect* _rect;
  SDL_Texture* _texture;
}COISprite;

COISprite* COISpriteCreate(int x, int y, int w, int h, SDL_Texture* texture);
void COISpriteDestroy(COISprite* sprite);

#endif
