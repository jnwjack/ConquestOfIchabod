#include "Battle.h"

COIBoard* battleCreateBoard(COIWindow* window, COIAssetLoader* loader, COIBoard* outsideBoard) {
  COIBoard* board = COIBoardCreate(99, 91, 95, 225, 640, 480, loader);
  COIBoardLoadSpriteMap(board, COIWindowGetRenderer(window), "src/battle/spritemap.dat");

  return board;
}
