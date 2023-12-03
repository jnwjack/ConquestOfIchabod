#include "COISprite.h"

int _pixelsToGridSquare(int px) {
  return px / COIBOARD_GRID_SIZE;
}

COISprite* COISpriteCreate(int x, int y, int w, int h, SDL_Texture* texture, int assetID) {
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
  sprite->_assetID = assetID;

  return sprite;
}

COISprite* COISpriteCopy(COISprite* original, COIAssetLoader* loader, SDL_Renderer* renderer) {
  COISprite* sprite = malloc(sizeof(COISprite));
  sprite->_x = original->_x;
  sprite->_y = original->_y;
  sprite->_width = original->_width;
  sprite->_height = original->_height;
  sprite->_visible = original->_visible;
  // JNW - here
  SDL_Surface* asset = COIAssetLoaderGetAsset(loader, original->_assetID);
  SDL_Texture* texture  = SDL_CreateTextureFromSurface(renderer, asset);
  sprite->_texture = texture;
  sprite->_drawRect = malloc(sizeof(SDL_Rect));
  sprite->_drawRect->x = sprite->_x;
  sprite->_drawRect->y = sprite->_y;
  sprite->_drawRect->w = sprite->_width;
  sprite->_drawRect->h = sprite->_height;
  sprite->_srcRect = NULL;
  sprite->_sheetCount = 0;
  sprite->_moving = false;
  sprite->_autoHandle = true;
  sprite->_extraCollision = NULL;

  return sprite;
}

COISprite* COISpriteCreateFromAssetID(int x, int y, int w, int h, COIAssetLoader* loader, int assetID, SDL_Renderer* renderer) {
  SDL_Surface* asset = COIAssetLoaderGetAsset(loader, assetID);
  SDL_Texture* texture  = SDL_CreateTextureFromSurface(renderer, asset);

  return COISpriteCreate(x, y, w, h, texture, assetID);
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

void COISpriteSetSheetIndex(COISprite* sprite, int row, int col) {
  if(sprite->_srcRect == NULL) {
    sprite->_srcRect = malloc(sizeof(SDL_Rect));
    // Default width and height to draw width and draw height
    sprite->_srcRect->w = COI_VIEW_WINDOW_WIDTH;
    sprite->_srcRect->h = COI_VIEW_WINDOW_HEIGHT;
  }
  // Assume sprite sheet is an n x 1 image of sprites
  sprite->_srcRect->x = col * sprite->_srcRect->w;
  sprite->_srcRect->y = row * sprite->_srcRect->h;
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
  // Check if we're in the same grid cell
  /*if ((sprite->_x/COIBOARD_GRID_SIZE) == (x/COIBOARD_GRID_SIZE) &&
      (sprite->_y/COIBOARD_GRID_SIZE) == (y/COIBOARD_GRID_SIZE)) {
    return COI_COLLISION;
  }
  
  return COI_NO_COLLISION;
  */
  

  int myGridX = _pixelsToGridSquare(x);
  int myGridY = _pixelsToGridSquare(y);

  int spriteGridLeft = _pixelsToGridSquare(sprite->_x);
  int spriteGridRight = _pixelsToGridSquare(sprite->_x + sprite->_width) - 1;
  int spriteGridTop = _pixelsToGridSquare(sprite->_y);
  int spriteGridBottom = _pixelsToGridSquare(sprite->_y + sprite->_height) - 1;

  if ((myGridX >= spriteGridLeft && myGridX <= spriteGridRight) &&
      (myGridY >= spriteGridTop && myGridY <= spriteGridBottom)) {
    if(sprite->_extraCollision != NULL) {
      int ecGridLeft = _pixelsToGridSquare(sprite->_x + sprite->_extraCollision->tlX);
      int ecGridRight = _pixelsToGridSquare(sprite->_x + sprite->_extraCollision->brX);
      int ecGridTop = _pixelsToGridSquare(sprite->_y + sprite->_extraCollision->tlY);
      int ecGridBottom = _pixelsToGridSquare(sprite->_y + sprite->_extraCollision->brY);
      if ((myGridX >= ecGridLeft && myGridX <= ecGridRight) &&
	  (myGridY >= ecGridTop && myGridY <= ecGridBottom)) {
	return sprite->_extraCollision->returnValue;
      }
    }
    return COI_COLLISION;
  }
  return COI_NO_COLLISION;
  
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

