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
  sprite->_sheetCount = 0;
  sprite->_moving = false;
  sprite->_autoHandle = true;
  sprite->_extraCollision = NULL;

  return sprite;
}

void COISpriteDestroy(COISprite* sprite) {
  SDL_DestroyTexture(sprite->_texture);
  free(sprite->_drawRect);
  if (sprite->_srcRect != NULL) {
    free(sprite->_srcRect);
  }
  if (sprite->_extraCollision != NULL) {
    free(sprite->_extraCollision);
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
  sprite->_sheetCount = 1;
}

void COISpriteSetSheetDimensions(COISprite* sprite, int w, int h) {
  if(sprite->_srcRect == NULL) {
    sprite->_srcRect = malloc(sizeof(SDL_Rect));
    sprite->_srcRect->x = 0;
    sprite->_srcRect->y = 0;
  }
  sprite->_srcRect->w = w;
  sprite->_srcRect->h = h;
  sprite->_sheetCount = sprite->_width / w;
}

void COISpriteSheetIncrementIndex(COISprite* sprite) {
  if (sprite->_srcRect == NULL) {
    return;
  }
  int currentIndex = sprite->_srcRect->x / sprite->_srcRect->w;
  int nextIndex = (currentIndex + 1) % sprite->_sheetCount; 
  sprite->_srcRect->x = nextIndex * sprite->_srcRect->w;
}

// Assumes that otherRect is smaller than or equal to the sprite's rect
int COISpriteCollision(COISprite* sprite, int x, int y, int width, int height) {
  int myTopLeft[] = { sprite->_x, sprite->_y };
  int myBottomRight[] = { sprite->_x + sprite->_width, sprite->_y + sprite->_height };
  int otherCorners[4][2] = {{ x, y },
			  { x, y + height },
			  { x + width, y },
			  { x + width, y + height }};

  bool collision = false;
  // Iterate through 4 corners of other rect and check if at least one is inside the box defined by
  // this sprite's top left and bottom right corners.
  int i, otherX, otherY;
  for (i = 0; i < 4; i++) {
    otherX = otherCorners[i][0];
    otherY = otherCorners[i][1];
    
    if (otherX >= myTopLeft[0] &&
	otherY >= myTopLeft[1] &&
	otherX <= myBottomRight[0] &&
	otherY <= myBottomRight[1]) {
      // Check for extra collision if one exists
      if(sprite->_extraCollision != NULL &&
	 otherX >= myTopLeft[0] + sprite->_extraCollision->tlX &&
	 otherY >= myTopLeft[1] + sprite->_extraCollision->tlY &&
	 otherX <= myTopLeft[0] + sprite->_extraCollision->brX &&
	 otherY <= myTopLeft[1] + sprite->_extraCollision->brY) {
	return sprite->_extraCollision->returnValue;
      }
      collision = true;
    }
  }

  if (!collision) {
    return COI_NO_COLLISION;
  }

  return COI_COLLISION;
}

void COISpriteSetExtraCollision(COISprite* sprite, COIExtraCollision* collision) {
  sprite->_extraCollision = collision;
}

void COISpriteSetPos(COISprite* sprite, int x, int y) {
  sprite->_x = x;
  sprite->_y = y;

  sprite->_drawRect->x = x;
  sprite->_drawRect->y = y;
}

