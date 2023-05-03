#include "Town.h"

COIBoard* townCreateBoard(COIWindow* window, COIAssetLoader* loader, PlayerInfo* pInfo) {
  COIBoard* board = COIBoardCreate(2, 132, 28, 255, 700, 700, loader);
  COIBoardLoadSpriteMap(board, COIWindowGetRenderer(window), "src/threadtown/spritemap.dat");
  TownContext* context = malloc(sizeof(TownContext));
  context->window = window;
  context->pInfo = pInfo;
  context->direction = MOVING_NONE;

  COIBoardSetContext(board, (void*)context);

  return board;
}

void townDestroyBoard(COIBoard* board) {
  COIBoardDestroy(board);
}
