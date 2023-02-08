// Should have a better system for includes
#include "engine/COIMenu.h"
#include "engine/COIWindow.h"
#include "inputloops.h"
#include "armory/Armory.h"
#include "items.h"
#include "tests.h"


int main(int argc, char** argv) {
  COIWindow* window = COIWindowCreate();

  // Global item data
  ItemList* itemList = loadItems();

  // Test inventory
  Inventory* inventory = createTestInventory(itemList);

  COIAssetLoader* loader = COIAssetLoaderCreate();
  COILoop threadTownLoop = &threadTown;
  void* context = malloc(sizeof(int) + sizeof(COIBoard*) + sizeof(COIWindow*));

  COIBoard* board = COIBoardCreate(2, 132, 28, 255, 700, 700);
  COIBoardLoadSpriteMap(board, loader, COIWindowGetRenderer(window), "src/threadtown/spritemap.dat");

  // Testing new strings
  COITextType* testType = COITextTypeCreate(25, 255, 255, 255, COIWindowGetRenderer(window));
  COIString* test = COIStringCreate("Turtle", 300, 300, testType);
  COIBoardSetStrings(board, &test, 1);

  COIBoard* armoryBoard = armoryCreateBoard(window, loader, board, inventory);

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
  
  armoryDestroy(armoryBoard->context);
  
  COIAssetLoaderDestroy(loader);
  // Test string
  COITextTypeDestroy(testType);
  COIStringDestroy(test);
  COIBoardDestroy(board);
  COIBoardDestroy(armoryBoard);
  COIWindowDestroy(window);
  ItemListDestroy(itemList);
  inventoryDestroy(inventory);

  return 0;
}
