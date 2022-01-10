#include "COIBoard.h"

COIBoard* COIBoardCreate(int r, int g, int b, int a, int w, int h) {
  COIBoard* board = malloc(sizeof(COIBoard));
  board->_bgColor[INDEX_RED] = r;
  board->_bgColor[INDEX_GREEN] = g;
  board->_bgColor[INDEX_BLUE] = b;
  board->_bgColor[INDEX_ALPHA] = a;
  board->_width = w;
  board->_height = h;
  board->_frameX = 0;
  board->_frameY = 0;
  board->_sprites = NULL;
  board->_spriteCount = 0;
  return board;
}

void COIBoardDestroy(COIBoard* board) {
  if (board->_sprites != NULL) {
    int i;
    for (i = 0; i < board->_spriteCount; i++) {
      free(board->_sprites[i]);
    }
    free(board->_sprites);
  }
  free(board);
}

int COIBoardBGColor(COIBoard* board, int index) {
  return board->_bgColor[index];
}

void COIBoardLoadSpriteMap(COIBoard* board, COIAssetLoader* loader,  SDL_Renderer* renderer, const char* filename) {
  FILE* fp;
  
  fp = fopen(filename, "r");
  if (fp == NULL) {
    return;
  }

  int spriteCount = countLines(filename);
  board->_sprites = malloc(spriteCount * sizeof(COISprite*));
  board->_spriteCount = spriteCount;
  printf("spritecount: %i", spriteCount);

  size_t len = 0;
  char* line;
  int assetID;
  int x;
  int y;
  int w;
  int h;
  int i = 0;
  while (getline(&line, &len, fp) != -1) {
    line[strcspn(line, "\n")] = '\0';

    assetID = atoi(strtok(line, " "));
    x = atoi(strtok(NULL, " "));
    y = atoi(strtok(NULL, " "));
    w = atoi(strtok(NULL, " "));
    h = atoi(strtok(NULL, " "));

    SDL_Surface* asset = COIAssetLoaderGetAsset(loader, assetID);
    SDL_Texture* texture  = SDL_CreateTextureFromSurface(renderer, asset);
    board->_sprites[i] = COISpriteCreate(x, y, w, h, texture);
    
    i++;
  }
}

COISprite** COIBoardGetSprites(COIBoard* board) {
  return board->_sprites;
}

int COIBoardGetSpriteCount(COIBoard* board) {
  return board->_spriteCount;
}

void COIBoardUpdateSpriteVisibility(COIBoard* board, int frameWidth, int frameHeight) {
  int farEdgeX = board->_frameX + frameWidth;
  int farEdgeY = board->_frameY + frameHeight;
  int i;
  COISprite* sprite;
  for (i = 0; i < board->_spriteCount; i++) {
    sprite = board->_sprites[i];
    if (((sprite->_x + sprite->_width) >= board->_frameX && sprite->_x <= farEdgeX)
	&& ((sprite->_y + sprite->_height) >= board->_frameY && sprite->_y <= farEdgeY)) {
      sprite->_visible = true;
      sprite->_rect->x = sprite->_x - board->_frameX;
      sprite->_rect->y = sprite->_y - board->_frameY;
    } else {
      sprite->_visible = false;
    }
  } 
}

bool COIBoardShiftFrameX(COIBoard* board, int stride, int frameWidth) {
  int newFrameX = board->_frameX + stride;
  if (newFrameX >= 0 && newFrameX + frameWidth <= board->_width) {
    board->_frameX = newFrameX;
    return true;
  }
  return false;
}

bool COIBoardShiftFrameY(COIBoard* board, int stride, int frameHeight) {
  int newFrameY = board->_frameY + stride;
  if (newFrameY >= 0 && newFrameY + frameHeight <= board->_height) {
    board->_frameY = newFrameY;
    return true;
  }
  return false;
}  
