#include <SDL2/SDL.h>
#include <stdbool.h>

#ifndef COISPRITE_H
#define COISPRITE_H

#define COI_NO_COLLISION 0
#define COI_COLLISION 1

typedef struct COIExtraCollision {
  int tlX;
  int tlY;
  int brX;
  int brY;
  int returnValue;
}COIExtraCollision;

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
  COIExtraCollision* _extraCollision;
}COISprite;

COISprite* COISpriteCreate(int x, int y, int w, int h, SDL_Texture* texture);
void COISpriteDestroy(COISprite* sprite);
void COISpriteSetSheetIndex(COISprite* sprite, int index);
void COISpriteSetSheetDimensions(COISprite* sprite, int w, int h);
void COISpriteSheetIncrementIndex(COISprite* sprite);
int COISpriteCollision(COISprite* sprite, int x, int y, int width, int height);
void COISpriteSetExtraCollision(COISprite* sprite,
				int topLeftX,
				int topLeftY,
				int bottomRightX,
				int bottomRightY,
				int returnValue);

#endif
