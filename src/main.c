// Should have a better system for includes
#include "engine/COIWindow.h"
#include "inputloops.h"


int main(int argc, char** argv) {
  COIWindow* window = COIWindowCreate();
  
  COIAssetLoader* loader = COIAssetLoaderCreate();
  COILoop threadTownLoop = &threadTown;
  int* context = malloc(sizeof(int));


  COIBoard* board = COIBoardCreate(2, 132, 28, 255, 700, 700);  

  COIBoardLoadSpriteMap(board, loader, COIWindowGetRenderer(window), "src/spritemaps/threadtown.dat");
  COIWindowSetLoop(window, threadTownLoop, (void*)context);
  COIWindowSetBoard(window, board);
  COIWindowLoop(window);

  free(context);
  COIAssetLoaderDestroy(loader);
  COIBoardDestroy(board);
  COIWindowDestroy(window);

  return 0;
}
