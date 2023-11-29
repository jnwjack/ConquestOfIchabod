// Should have a better system for includes
#include "engine/COIMenu.h"
#include "engine/COIWindow.h"
#include "inputloops.h"
#include "armory/Armory.h"
#include "items.h"
#include "tests.h"
#include "player.h"
#include "threadtown/Town.h"

int main(int argc, char** argv) {
  COIWindow* window = COIWindowCreate();
  COIAssetLoader* loader = COIAssetLoaderCreate();

  // Global item data
  ItemList* itemList = loadItems();

  // Test inventory
  Inventory* inventory = createTestInventory(itemList);

  // Initialize player data
  COISprite* playerSprite = COISpriteCreateFromAssetID(320, 320, 32, 32, loader, 1, COIWindowGetRenderer(window));
  PlayerInfo* pInfo = playerInfoCreate("Wique", playerSprite, inventory);

  
  COIBoard* townBoard = townCreateBoard(window, loader, pInfo);
  COILoop threadTownLoop = &threadTown;
  COIWindowSetBoard(window, townBoard, threadTownLoop);
  COIWindowLoop(window);

  /*void* context = malloc(sizeof(int) + sizeof(COIBoard*) + sizeof(COIWindow*));


  COIBoard* board = COIBoardCreate(2, 132, 28, 255, 700, 700, loader);
  COIBoardLoadSpriteMap(board, COIWindowGetRenderer(window), "src/threadtown/spritemap.dat");

  COIBoard* armoryBoard = armoryCreateBoard(window, loader, board, inventory);

  // Modify threadtown context
  COIBoard** armoryBoardPtr = (COIBoard**) (context + sizeof(int));
  *armoryBoardPtr = armoryBoard;
  COIWindow** windowPtr = (COIWindow**) (context + sizeof(int) + sizeof(COIBoard*));
  *windowPtr = window;
  COIBoardSetContext(board, context);
  
  COIWindowSetBoard(window, board, threadTownLoop);
  COIWindowLoop(window);*/


  // Cleanup
  COIAssetLoaderDestroy(loader);
  COIBoardDestroy(townBoard);
  COIWindowDestroy(window);
  ItemListDestroy(itemList);
  inventoryDestroy(inventory);

  return 0;
}
