#ifndef COISPRITE_H
#define COISPRITE_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include "COIExtraCollision.h"

#define COI_NO_COLLISION 0
#define COI_COLLISION 1

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
  bool _moving;
  // true if COIBoard manages visibility of sprite
  bool _autoHandle;
  COIExtraCollision* _extraCollision;
}COISprite;

COISprite* COISpriteCreate(int x, int y, int w, int h, SDL_Texture* texture);
void COISpriteDestroy(COISprite* sprite);
void COISpriteSetSheetIndex(COISprite* sprite, int index);
void COISpriteSetSheetDimensions(COISprite* sprite, int w, int h);
void COISpriteSheetIncrementIndex(COISprite* sprite);
int COISpriteCollision(COISprite* sprite, int x, int y, int width, int height);
void COISpriteSetExtraCollision(COISprite* sprite, COIExtraCollision* collision);

#endif
