#include "COIBoard.h"

COIBoard* COIBoardCreate(int r, int g, int b, int a, int w, int h, COIAssetLoader* loader) {
  COIBoard* board = malloc(sizeof(COIBoard));
  board->_bgColor[INDEX_RED] = r;
  board->_bgColor[INDEX_GREEN] = g;
  board->_bgColor[INDEX_BLUE] = b;
  board->_bgColor[INDEX_ALPHA] = a;
  board->_width = w;
  board->_height = h;
  board->_frameX = 0;
  board->_frameY = 0;
  board->_frameWidth = 0;
  board->_frameHeight = 0;
  board->_sprites = NULL;
  board->_spriteCount = 0;
  board->_shouldDraw = true;
  board->context = NULL;
  board->strings = NULL;
  board->stringCount = 0;
  board->loader = loader;

  return board;
}

void COIBoardDestroy(COIBoard* board) {
  if (board == NULL){
    return;
  }
  if (board->strings != NULL) {
    free(board->strings);
  }
  if (board->_sprites != NULL) {
    int i;
    for (i = 0; i < board->_spriteCount; i++) {
      if (board->_sprites[i] != NULL) {
	free(board->_sprites[i]);
      }
    }
    free(board->_sprites);
  }
}

int COIBoardBGColor(COIBoard* board, int index) {
  return board->_bgColor[index];
}

void COIBoardLoadSpriteMap(COIBoard* board, SDL_Renderer* renderer, const char* filename) {
  FILE* fp = NULL;
  
  fp = fopen(filename, "r");
  if (fp == NULL) {
    return;
  }

  int spriteCount = countLines(filename);
  board->_sprites = malloc(spriteCount * sizeof(COISprite*));
  board->_spriteCount = spriteCount;

  size_t len = 0;
  char* line = NULL;
  int assetID;
  int x;
  int y;
  int w;
  int h;
  int i = 0;
  COISprite* sprite = NULL;
  while (getline(&line, &len, fp) != -1) {
    line[strcspn(line, "\n")] = '\0';

    assetID = atoi(strtok(line, " "));
    x = atoi(strtok(NULL, " "));
    y = atoi(strtok(NULL, " "));
    w = atoi(strtok(NULL, " "));
    h = atoi(strtok(NULL, " "));

    SDL_Surface* asset = COIAssetLoaderGetAsset(board->loader, assetID);
    SDL_Texture* texture  = SDL_CreateTextureFromSurface(renderer, asset);
    sprite = COISpriteCreate(x, y, w, h, texture);
    COISpriteSetExtraCollision(sprite, COIAssetLoaderGetCollision(board->loader, assetID));
    board->_sprites[i] = sprite;
    i++;
  }
}

COISprite** COIBoardGetSprites(COIBoard* board) {
  return board->_sprites;
}

int COIBoardGetSpriteCount(COIBoard* board) {
  return board->_spriteCount;
}

void COIBoardUpdateSpriteVisibility(COIBoard* board) {
  int farEdgeX = board->_frameX + board->_frameWidth;
  int farEdgeY = board->_frameY + board->_frameHeight;
  int i;
  COISprite* sprite = NULL;
  for (i = 0; i < board->_spriteCount; i++) {
    sprite = board->_sprites[i];
    if (((sprite->_x + sprite->_width) >= board->_frameX && sprite->_x <= farEdgeX)
	&& ((sprite->_y + sprite->_height) >= board->_frameY && sprite->_y <= farEdgeY)) {
      if (sprite->_autoHandle) {
	sprite->_visible = true;
      }
      sprite->_drawRect->x = sprite->_x - board->_frameX;
      sprite->_drawRect->y = sprite->_y - board->_frameY;
    } else {
      if (sprite->_autoHandle) {
	sprite->_visible = false;
      }
    }
  }
}

bool COIBoardShiftFrameX(COIBoard* board, int stride) {
  int newFrameX = board->_frameX + stride;
  if (newFrameX >= 0 && newFrameX + board->_frameWidth <= board->_width) {
    board->_frameX = newFrameX;
    board->_shouldDraw = true;
    return true;
  }
  return false;
}

bool COIBoardShiftFrameY(COIBoard* board, int stride) {
  int newFrameY = board->_frameY + stride;
  if (newFrameY >= 0 && newFrameY + board->_frameHeight <= board->_height) {
    board->_frameY = newFrameY;
    board->_shouldDraw = true;
    return true;
  }
  return false;
}

// Sprite should be member of this board's sprite list
void COIBoardMoveSprite(COIBoard* board, COISprite* sprite, int x, int y) {
  int newX = sprite->_x + x;
  int newY = sprite->_y + y;
  if (newX >= 0 && newX + sprite->_width <= board->_width) {
    sprite->_x = newX;
    board->_shouldDraw = true;
  }
  if (newY >= 0 && newY + sprite->_height <= board->_height) {
    sprite->_y = newY;
    board->_shouldDraw = true;
  }
}

void COIBoardQueueDraw(COIBoard* board) {
  board->_shouldDraw = true;
}

void COIBoardSetContext(COIBoard* board, void* context) {
  board->context = context;
}

void COIBoardSetStrings(COIBoard* board, COIString** strings, int count) {
  if (board->strings != NULL) {
    free(board->strings);
  }

  board->stringCount = count;
  board->strings = malloc(sizeof(COIString*) * board->stringCount);
  for (int i = 0; i < board->stringCount; i++) {
    board->strings[i] = strings[i];
  }
  
  /*  board->strings = strings;
      board->stringCount = count;*/
}

