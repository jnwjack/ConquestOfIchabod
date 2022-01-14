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
  SDL_Rect* _drawRect;
  SDL_Rect* _srcRect;
  SDL_Texture* _texture;
  int _sheetCount;
}COISprite;

COISprite* COISpriteCreate(int x, int y, int w, int h, SDL_Texture* texture);
void COISpriteDestroy(COISprite* sprite);
void COISpriteSetSheetIndex(COISprite* sprite, int index);
void COISpriteSetSheetDimensions(COISprite* sprite, int w, int h);
void COISpriteSheetIncrementIndex(COISprite* sprite);

#endif
