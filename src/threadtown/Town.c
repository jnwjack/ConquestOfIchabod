#include "Town.h"

COIBoard* townCreateBoard(COIWindow* window, COIAssetLoader* loader, PlayerInfo* pInfo) {
  COIBoard* board = COIBoardCreate(2, 132, 28, 255, 700, 700, loader);
  COIBoardLoadSpriteMap(board, COIWindowGetRenderer(window), "src/threadtown/spritemap.dat");
  TownContext* context = malloc(sizeof(TownContext));
  context->window = window;
  context->pInfo = pInfo;
  context->direction = MOVING_NONE;
  // Only 1 dynamic sprite: the player
  COISprite** dynSprites = actorGetSpriteList(context->pInfo->party, 1);
  COIBoardSetDynamicSprites(board, dynSprites, 1);

  COIBoardSetContext(board, (void*)context);

  return board;
}

void townDestroyBoard(COIBoard* board) {
  COIBoardDestroy(board);
}
