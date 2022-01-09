// Should have a better system for includes
#include "engine/COIWindow.h"
#include "engine/COIBoard.h"

int main(int argc, char** argv) {
  COIAssetLoader* loader = COIAssetLoaderCreate("src/engine/etc/mapping.cfg");

  COIWindow* window = COIWindowCreate();
  COIBoard* board = COIBoardCreate(230, 25, 25, 255);

  COIBoardLoadSpriteMap(board, loader, COIWindowGetRenderer(window), "src/spritemaps/turtles.dat");
  COIWindowSetBoard(window, board);
  COIWindowLoop(window);

  COIAssetLoaderDestroy(loader);
  COIBoardDestroy(board);
  COIWindowDestroy(window);

  return 0;
}
