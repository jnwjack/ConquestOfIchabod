// Should have a better system for includes
#include "engine/COIWindow.h"
#include "inputloops.h"


int main(int argc, char** argv) {
  COIWindow* window = COIWindowCreate();
  
  COIAssetLoader* loader = COIAssetLoaderCreate();
  COILoop threadTownLoop = &threadTown;
  void* context = malloc(sizeof(int) + sizeof(COIBoard*) + sizeof(COIWindow*));


  COIBoard* board = COIBoardCreate(2, 132, 28, 255, 700, 700);  
  COIBoardLoadSpriteMap(board, loader, COIWindowGetRenderer(window), "src/spritemaps/threadtown.dat");

  COIBoard* armoryBoard = COIBoardCreate(99, 91, 95, 255, 640, 480);
  COILoop armoryLoop = &armory;
  COIBoardLoadSpriteMap(armoryBoard, loader, COIWindowGetRenderer(window), "src/spritemaps/armory.dat");

  COIBoard** armoryBoardPtr = (COIBoard**) (context + sizeof(int));
  *armoryBoardPtr = armoryBoard;

  COIWindow** windowPtr = (COIWindow**) (context + sizeof(int) + sizeof(COIBoard*));
  *windowPtr = window;

  COIWindowSetLoop(window, threadTownLoop, context);
  COIWindowSetBoard(window, board);
  COIWindowLoop(window);

  // Only need to free the int, other pointers in context get handled elsewhere
  int* contextIntPtr = (int*) (context);
  free(contextIntPtr);
  COIAssetLoaderDestroy(loader);
  COIBoardDestroy(board);
  COIBoardDestroy(armoryBoard);
  COIWindowDestroy(window);

  return 0;
}
