#include "COIBoard.h"

COIBoard* COIBoardCreate(int r, int g, int b, int a, int w, int h, COIAssetLoader* loader) {
  COIBoard* board = malloc(sizeof(COIBoard));
  board->spriteGrid = NULL;
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
  board->persistentSprites = NULL;
  board->perSpriteCount = 0;
  board->dynamicSprites = LinkedListCreate();
  board->_shouldDraw = true;
  board->context = NULL;
  // JNW
  board->strings = malloc(sizeof(COIString*) * COIBOARD_MAX_STRINGS);
  for (int i = 0; i < COIBOARD_MAX_STRINGS; i++) {
    board->strings[i] = NULL;
  }
  //board->strings = NULL;
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
  if (board->spriteGrid) {
    free(board->spriteGrid);
    printf("freed spritegrid\n");
  }
  if (board->persistentSprites != NULL) {
    for (int i = 0; i < board->perSpriteCount; i++) {
      // Dynamic sprites are used elsewhere
      //free(board->persistentSprites[i]);
    }
    free(board->persistentSprites);
  }
  LinkedListDestroy(board->dynamicSprites);
  free(board);
}

int COIBoardBGColor(COIBoard* board, int index) {
  // Scale color based on phase of day.
  // pix_value_scaled = pix_value / (2^p)
  // where p = { 0, 1, 2, 3 } for each of { MORNING, DAY, EVENING, NIGHT }
  return board->_bgColor[index] / POW_INT(2, GLOBAL_TIME.phase);

  // May want to do something more specific later, but this is fine for now.
}

void COIBoardLoadSpriteMap(COIBoard* board, SDL_Renderer* renderer, const char* filename) {
  FILE* fp = NULL;
  
  fp = fopen(filename, "r");
  if (fp == NULL) {
    return;
  }

  int spriteCount = countLines(filename);
  COISprite** sprites = malloc(spriteCount * sizeof(COISprite*));

  size_t len = 0;
  char* line = NULL;
  int assetID;
  int x;
  int y;
  int w;
  int h;
  int i = 0;
  // left off here: track max x and y to get width and height of board
  // then, malloc an entire grid equal to the width / grid_cell_width x height / grid_cell_height
  // Load sprites from spritemap into relevant position in grid
  // During runtime, keep track of the relevant grid position that corresponds to top left of viewport.
  // Only have to iterate through (640 / 32) x (480 / 32) sprites per frame now
  COISprite* sprite = NULL;
  int maxGridX = -1;
  int maxGridY = -1;
  while (getline(&line, &len, fp) != -1) {
    line[strcspn(line, "\n")] = '\0';

    assetID = atoi(strtok(line, " "));
    x = atoi(strtok(NULL, " "));
    y = atoi(strtok(NULL, " "));
    w = atoi(strtok(NULL, " "));
    h = atoi(strtok(NULL, " "));
    int gridX = x / COIBOARD_GRID_SIZE;
    int gridY = y / COIBOARD_GRID_SIZE;
    if (gridX > maxGridX) {
      maxGridX = gridX;
    }
    if (gridY > maxGridY) {
      maxGridY = gridY;
    }
    SDL_Surface* asset = COIAssetLoaderGetAsset(board->loader, assetID);
    SDL_Texture* texture  = SDL_CreateTextureFromSurface(renderer, asset);
    sprite = COISpriteCreate(x, y, w, h, texture, assetID);
    COISpriteSetExtraCollision(sprite, COIAssetLoaderGetCollision(board->loader, assetID));
    sprites[i] = sprite;
    i++;
  }

  board->spriteGridWidth = maxGridX + 1;
  board->spriteGridHeight = maxGridY + 1;
  int totalGridSquares = board->spriteGridWidth * board->spriteGridHeight;
  board->spriteGrid = malloc(totalGridSquares * sizeof(COISprite*));

  printf("grid size: %i\n", totalGridSquares);
  for (int i = 0; i < totalGridSquares; i++) {
    board->spriteGrid[i] = NULL;
  }

  for (int i = 0; i < spriteCount; i++) {
    COISprite* sprite = sprites[i];
    int xOrigin = (sprite->_x / COIBOARD_GRID_SIZE);
    int yOrigin = (sprite->_y / COIBOARD_GRID_SIZE);

    // Insert sprite into it's position in the grid.
    // Also place it in nearby cells if it's bigger than 1x1.
    for (int yDelta = 0; yDelta < sprite->_height / COIBOARD_GRID_SIZE; yDelta++) {
      for (int xDelta = 0; xDelta < sprite->_width / COIBOARD_GRID_SIZE; xDelta++) {
        int index = (yOrigin + yDelta) * board->spriteGridWidth + (xOrigin + xDelta);
        board->spriteGrid[index] = sprite;
        sprite->_visible = false;
      }
    }
  }

  if (line) {
    free(line);
  }

  free(sprites);
  fclose(fp);
}

void COIBoardSetPersistentSprites(COIBoard* board, COISprite** sprites, int count) {
  if (board->persistentSprites != NULL) {
    for (int i = 0; i < board->perSpriteCount; i++) {
      // Dynamic sprites are used elsewhere
      // free(board->persistentSprites[i]);
    }
    free(board->persistentSprites);
  }
  board->persistentSprites = sprites;
  board->perSpriteCount = count;
}

void COIBoardAdjustSprite(COIBoard* board, COISprite* sprite) {
  int farEdgeX = board->_frameX + board->_frameWidth;
  int farEdgeY = board->_frameY + board->_frameHeight;

  if (sprite->_autoHandle) {
    if (((sprite->_x + sprite->_width) >= board->_frameX && sprite->_x <= farEdgeX)
	  && ((sprite->_y + sprite->_height) >= board->_frameY && sprite->_y <= farEdgeY)) {
      sprite->_drawRect->x = sprite->_x - board->_frameX;
      sprite->_drawRect->y = sprite->_y - board->_frameY;
      sprite->_visible = true;
    } else {
      sprite->_visible = false;
    }
  }
}

void COIBoardUpdateSpriteVisibility(COIBoard* board) {
  // Persistent sprites
  for (int i = 0; i < board->perSpriteCount; i++) {
    COIBoardAdjustSprite(board, board->persistentSprites[i]);
  }

  // Dynamic sprites
  LinkedListResetCursor(board->dynamicSprites);
  COISprite* sprite = (COISprite*)LinkedListNext(board->dynamicSprites);
  while (sprite != NULL) {
    COIBoardAdjustSprite(board, sprite);
    sprite = (COISprite*)LinkedListNext(board->dynamicSprites);
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
  }
  if (newY >= 0 && newY + sprite->_height <= board->_height) {
    sprite->_y = newY;
  }
  // If autohandle is off, user-defined position is also the relative position on the screen.
  // We do no adjustment on it.
  if (!sprite->_autoHandle) {
    sprite->_drawRect->x = sprite->_x;
    sprite->_drawRect->y = sprite->_y;
  } else {
    COIBoardAdjustSprite(board, sprite);
    if (sprite->_visible) {
      board->_shouldDraw = true;
    }
  }
}

void COIBoardQueueDraw(COIBoard* board) {
  board->_shouldDraw = true;
}

void COIBoardSetContext(COIBoard* board, void* context) {
  board->context = context;
}

bool COIBoardAddString(COIBoard* board, COIString* string) {
  if (board->stringCount == COIBOARD_MAX_STRINGS) {
    return false;
  }

  board->strings[board->stringCount] = string;
  string->index = board->stringCount;
  board->stringCount++;
  return true;
}

void COIBoardRemoveString(COIBoard* board, COIString* string) {
  if (string->index == -1) {
    return;
  }

  board->strings[string->index] = NULL;
  int holeIndex = string->index;

  // Shift all strings at a higher index down a position to fill hole
  for (int i = holeIndex + 1; i < board->stringCount; i++) {
    board->strings[i - 1] = board->strings[i];
    board->strings[i - 1]->index = i - 1;
    board->strings[i] = NULL;
  }

  board->stringCount -= 1;
  string->index = -1;
}

void COIBoardSetStrings(COIBoard* board, COIString** strings, int count) {
  /*if (board->strings != NULL) {
    free(board->strings);
    }*/

  board->stringCount = count;
  //board->strings = malloc(sizeof(COIString*) * board->stringCount);
  for (int i = 0; i < board->stringCount; i++) {
    board->strings[i] = strings[i];
  }
  
  /*  board->strings = strings;
      board->stringCount = count;*/
}

void COIBoardAddDynamicSprite(COIBoard* board, COISprite* sprite) {
  LinkedListAdd(board->dynamicSprites, (void*)sprite);
}

void COIBoardRemoveDynamicSprite(COIBoard* board, COISprite* sprite) {
  LinkedListRemove(board->dynamicSprites, (void*)sprite);
}

