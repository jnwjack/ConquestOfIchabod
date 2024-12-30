#include "COISprite.h"

int _pixelsToGridSquare(int px) {
  return px / COIBOARD_GRID_SIZE;
}

// Improvement: In cbb, option to set animation duration, etc.
// Better place to put this?
void COISpriteAnimateInPlace(COISprite* sprite) {
  if (sprite->_assetID == 21) {
    if (sprite->_srcRect) {
      sprite->_animationTicks++;
      if (sprite->_animationTicks > 4) {
	int oldCol = sprite->_srcRect->x / sprite->_srcRect->w;
	COISpriteSetSheetIndex(sprite, 0, (oldCol + 1) % 3);
	sprite->_animationTicks = 0;
      }
    } 
  }
  
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
  sprite->alwaysAnimate = false;
  sprite->_animationTicks = 0;
  sprite->_autoHandle = true;
  sprite->_extraCollision = NULL;
  sprite->_assetID = assetID;
  sprite->viewWindowWidth = COI_VIEW_WINDOW_WIDTH;
  sprite->viewWindowHeight = COI_VIEW_WINDOW_HEIGHT;

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
  sprite->alwaysAnimate = false;
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
  free(sprite);
}

void COISpriteSetSheetIndex(COISprite* sprite, int row, int col) {
  if(sprite->_srcRect == NULL) {
    sprite->_srcRect = malloc(sizeof(SDL_Rect));
    // Default width and height to draw width and draw height
    sprite->_srcRect->w = sprite->viewWindowWidth;
    sprite->_srcRect->h = sprite->viewWindowHeight;;
  }
  // Assume sprite sheet is an n x 1 image of sprites
  sprite->_srcRect->x = col * sprite->_srcRect->w;
  sprite->_srcRect->y = row * sprite->_srcRect->h;
  sprite->_sheetCount = 1;
}

bool COISpriteHasMultipleFrames(COISprite* sprite) {
  return sprite->_srcRect != NULL;
}

/*void COISpriteSheetIncrementIndex(COISprite* sprite) {
  if (sprite->_srcRect == NULL) {
    return;
  }
  int currentIndex = sprite->_srcRect->x / sprite->_srcRect->w;
  int nextIndex = (currentIndex + 1) % sprite->_sheetCount; 
  sprite->_srcRect->x = nextIndex * sprite->_srcRect->w;
}
*/

// Assumes that otherRect is smaller than or equal to the sprite's rect
int COISpriteCollision(COISprite* sprite, int x, int y, int width, int height) {
  // Check if we're in the same grid cell

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

