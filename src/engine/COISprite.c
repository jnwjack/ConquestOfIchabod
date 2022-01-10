#include "COISprite.h"

COISprite* COISpriteCreate(int x, int y, int w, int h, SDL_Texture* texture) {
  COISprite* sprite = malloc(sizeof(COISprite));
  sprite->_x = x;
  sprite->_y = y;
  sprite->_width = w;
  sprite->_height = h;
  sprite->_visible = false;
  sprite->_texture = texture;
  sprite->_rect = malloc(sizeof(SDL_Rect));
  sprite->_rect->x = x;
  sprite->_rect->y = y;
  sprite->_rect->w = w;
  sprite->_rect->h = h;

  return sprite;
}

void COISpriteDestroy(COISprite* sprite) {
  SDL_DestroyTexture(sprite->_texture);
  free(sprite->_rect);
  free(sprite);
}
