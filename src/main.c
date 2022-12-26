// Should have a better system for includes
#include "engine/COIWindow.h"
#include "engine/COIMenu.h"
#include "inputloops.h"
#include "armory/Armory.h"
#include "items.h"


int main(int argc, char** argv) {
  COIWindow* window = COIWindowCreate();

  // Global item data
  ItemList* itemList = loadItems();

  COIAssetLoader* loader = COIAssetLoaderCreate();
  COILoop threadTownLoop = &threadTown;
  void* context = malloc(sizeof(int) + sizeof(COIBoard*) + sizeof(COIWindow*));

  COIBoard* board = COIBoardCreate(2, 132, 28, 255, 700, 700, 0);
  COIBoardLoadSpriteMap(board, loader, COIWindowGetRenderer(window), "src/threadtown/spritemap.dat");


  COIBoard* armoryBoard = armoryCreateBoard(window, loader, board);

  // Modify threadtown context
  COIBoard** armoryBoardPtr = (COIBoard**) (context + sizeof(int));
  *armoryBoardPtr = armoryBoard;
  COIWindow** windowPtr = (COIWindow**) (context + sizeof(int) + sizeof(COIBoard*));
  *windowPtr = window;
  COIBoardSetContext(board, context);
  
  COIWindowSetBoard(window, board, threadTownLoop);
  COIWindowLoop(window);


  // Cleanup
  // Only need to free the int, other pointers in context get handled elsewhere
  int* contextIntPtr = (int*) (context);
  free(contextIntPtr);
  
  armoryDestroy(armoryBoard->_context);
  
  COIAssetLoaderDestroy(loader);
  COIBoardDestroy(board);
  COIBoardDestroy(armoryBoard);
  COIWindowDestroy(window);
  ItemListDestroy(itemList);

  return 0;
}
