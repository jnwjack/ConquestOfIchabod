#ifndef COISPRITE_H
#define COISPRITE_H

#ifdef __WINDOWS__
#define SDL_MAIN_HANDLED
#endif
#include <SDL2/SDL.h>
#include <stdbool.h>
#include "COIExtraCollision.h"
#include "COIAssetLoader.h"

// Used when moving view window on sprite sheets
#define COI_VIEW_WINDOW_WIDTH 32
#define COI_VIEW_WINDOW_HEIGHT 32

typedef struct COISprite {
  int _x;
  int _y;
  int _width;
  int _height;
  int viewWindowWidth;
  int viewWindowHeight;
  bool _visible;
  SDL_Rect* _drawRect;
  SDL_Rect* _srcRect;
  SDL_Texture* _texture;
  int _sheetCount;
  bool alwaysAnimate;
  unsigned int _animationTicks;
  // true if COIBoard manages visibility of sprite
  bool _autoHandle;
  COIExtraCollision* _extraCollision;
  int _assetID;
  bool _extraCollisionActive; // Extra collision should only trigger if the sprite has JUST moved in contact with the other sprite
}COISprite;

COISprite* COISpriteCreate(int x, int y, int w, int h, SDL_Texture* texture, int assetID);
COISprite* COISpriteCopy(COISprite* original, COIAssetLoader* loader, SDL_Renderer* renderer);
COISprite* COISpriteCreateFromAssetID(int x, int y, int w, int h, COIAssetLoader* loader, int assetID, SDL_Renderer* renderer);
void COISpriteDestroy(COISprite* sprite);
void COISpriteSetSheetIndex(COISprite* sprite, int row, int col);
//void COISpriteSheetIncrementIndex(COISprite* sprite);
int COISpriteCollision(COISprite* sprite, int x, int y, int width, int height);
void COISpriteSetExtraCollision(COISprite* sprite, COIExtraCollision* collision);
void COISpriteSetPos(COISprite* sprite, int x, int y);
void COISpriteAnimateInPlace(COISprite* sprite);

#endif
