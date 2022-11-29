#include "COIBoard.h"

COIBoard* COIBoardCreate(int r, int g, int b, int a, int w, int h, int numTextGroups) {
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
  board->_context = NULL;
  board->_textGroupCount = numTextGroups;
  if (numTextGroups > 0) {
    board->_textGroups = malloc(numTextGroups * sizeof(COITextGroup*));
  } else {
    board->_textGroups = NULL;
  }
  
  return board;
}

void COIBoardDestroy(COIBoard* board) {
  if (board == NULL){
    return;
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
  if (board->_textGroups != NULL) {
    free(board->_textGroups);
  }
}

int COIBoardBGColor(COIBoard* board, int index) {
  return board->_bgColor[index];
}

void COIBoardLoadSpriteMap(COIBoard* board, COIAssetLoader* loader,  SDL_Renderer* renderer, const char* filename) {
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

    SDL_Surface* asset = COIAssetLoaderGetAsset(loader, assetID);
    SDL_Texture* texture  = SDL_CreateTextureFromSurface(renderer, asset);
    sprite = COISpriteCreate(x, y, w, h, texture);
    COISpriteSetExtraCollision(sprite, COIAssetLoaderGetCollision(loader, assetID));
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

  // User handles text visibility
  /*COIText** texts;
  COIText* text;
  for (i = 0; i < board->_textGroupCount; i++) {
    texts = COITextGroupGetTexts(board->_textGroups[i]);
    int j;
    for (j = 0; j < COITextGroupGetTextCount(board->_textGroups[i]); j++) {
      text = texts[j];
      if (((text->_x + text->_width) >= board->_frameX && text->_x <= farEdgeX)
	&& ((text->_y + text->_height) >= board->_frameY && text->_y <= farEdgeY)){
	text->_visible = true;
	text->_drawRect->x = text->_x - board->_frameX;
	text->_drawRect->y = text->_y - board->_frameY;
      } else {
	text->_visible = false;
      }
    }
    }*/
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

void COIBoardSetContext(COIBoard* board, void* context) {
  board->_context = context;
}

void COIBoardAddTextGroup(COIBoard* board, COITextGroup* group, int index) {
  board->_textGroups[index] = group;
}

COIText** COIBoardGetTexts(COIBoard* board, int index) {
  return COITextGroupGetTexts(board->_textGroups[index]);
}

int COIBoardGetTextCount(COIBoard* board, int index) {
  return COITextGroupGetTextCount(board->_textGroups[index]);
}

int COIBoardGetTextGroupCount(COIBoard* board) {
  return board->_textGroupCount;
}

