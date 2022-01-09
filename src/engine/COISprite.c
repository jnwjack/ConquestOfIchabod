#include "COISprite.h"

COISprite* COISpriteCreate(int x, int y, struct SDL_Texture* texture) {
  COISprite* sprite = malloc(sizeof(COISprite));
  sprite->_x = x;
  sprite->_y = y;
  sprite->_texture = texture;
  return sprite;
}

void COISpriteDestroy(COISprite* sprite) {
  SDL_DestroyTexture(sprite->_texture);
  free(sprite);
}
