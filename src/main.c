// Should have a better system for includes
#include "engine/COIWindow.h"
#include "inputloops.h"


int main(int argc, char** argv) {
  COIWindow* window = COIWindowCreate();
  
  COIAssetLoader* loader = COIAssetLoaderCreate();
  COILoop threadTownLoop = &threadTown;
  void* context = malloc(sizeof(int) + sizeof(COIBoard*) + sizeof(COIWindow*));

  COITextGroup* mediumGroup = COITextGroupCreate(25, 255, 255, 255, "src/armory/text.dat", COIWindowGetRenderer(window));

  COIBoard* board = COIBoardCreate(2, 132, 28, 255, 700, 700, 0);
  COIBoardLoadSpriteMap(board, loader, COIWindowGetRenderer(window), "src/threadtown/spritemap.dat");

  COIBoard* armoryBoard = COIBoardCreate(99, 91, 95, 255, 640, 480, 1);
  COIBoardAddTextGroup(armoryBoard, mediumGroup, 0);
  COILoop armoryLoop = &armory;
  COIBoardLoadSpriteMap(armoryBoard, loader, COIWindowGetRenderer(window), "src/armory/spritemap.dat");

  // Modify threadtown context
  COIBoard** armoryBoardPtr = (COIBoard**) (context + sizeof(int));
  *armoryBoardPtr = armoryBoard;
  COIWindow** windowPtr = (COIWindow**) (context + sizeof(int) + sizeof(COIBoard*));
  *windowPtr = window;
  COIBoardSetContext(board, context);

  // Armory context
  void* armoryContext = malloc(sizeof(int) + sizeof(COIBoard*) + sizeof(COIWindow*));
  *(int*)armoryContext = 0;
  *(COIBoard**) (armoryContext + sizeof(int)) = board;
  *(COIWindow**) (armoryContext  + sizeof(int) + sizeof(COIBoard*)) = window;
  COIBoardSetContext(armoryBoard, armoryContext);
  
  
  COIWindowSetBoard(window, board, threadTownLoop);
  COIWindowLoop(window);

  // Cleanup

  // Only need to free the int, other pointers in context get handled elsewhere
  int* contextIntPtr = (int*) (context);
  free(contextIntPtr);
  contextIntPtr = (int*) (armoryContext);
  free(contextIntPtr);
  
  COITextGroupDestroy(mediumGroup);
  COIAssetLoaderDestroy(loader);
  COIBoardDestroy(board);
  COIBoardDestroy(armoryBoard);
  COIWindowDestroy(window);

  return 0;
}
