#include "COISprite.h"

COISprite* COISpriteCreate(int x, int y, int w, int h, SDL_Texture* texture) {
  COISprite* sprite = malloc(sizeof(COISprite));
  sprite->_x = x;
  sprite->_y = y;
  sprite->_width = w;
  sprite->_height = h;
  sprite->_visible = false;
  sprite->_texture = texture;
  sprite->_drawRect = malloc(sizeof(SDL_Rect));
  sprite->_drawRect->x = x;
  sprite->_drawRect->y = y;
  sprite->_drawRect->w = w;
  sprite->_drawRect->h = h;
  sprite->_srcRect = NULL;

  return sprite;
}

void COISpriteDestroy(COISprite* sprite) {
  SDL_DestroyTexture(sprite->_texture);
  free(sprite->_drawRect);
  if (sprite->_srcRect) {
    free(sprite->_srcRect);
  }
  free(sprite);
}

void COISpriteSetSheetIndex(COISprite* sprite, int index) {
  if(sprite->_srcRect == NULL) {
    sprite->_srcRect = malloc(sizeof(SDL_Rect));
    // Default width and height to draw width and draw height
    sprite->_srcRect->w = sprite->_width;
    sprite->_srcRect->h = sprite->_height;
    sprite->_srcRect->y = 0;
  }
  // Assume sprite sheet is an n x 1 image of sprites
  sprite->_srcRect->x = index * sprite->_width;
}

void COISpriteSetSheetDimensions(COISprite* sprite, int w, int h) {
  if(sprite->_srcRect == NULL) {
    sprite->_srcRect = malloc(sizeof(SDL_Rect));
    sprite->_srcRect->x = 0;
    sprite->_srcRect->y = 0;
  }
  sprite->_srcRect->w = w;
  sprite->_srcRect->h = h;
}
