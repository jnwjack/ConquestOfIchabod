// Should have a better system for includes
#include "inputloops.h"
#include "armory/Armory.h"
#include "items.h"
#include "tests.h"
#include "player.h"
#include "threadtown/Town.h"

int main(int argc, char** argv) {
  // Initialize global window and asset loader
  COIWindowInit();
  COIAssetLoaderInit();
  
  // In the future, we can remove COIWindow member variables from structs so
  // we don't have to pass it around all over the place.
  COIWindow* window = COI_GLOBAL_WINDOW;
  // Same for COIAssetLoader
  COIAssetLoader* loader = COI_GLOBAL_LOADER;

  // Global item data
  ItemList* itemList = loadItems();

  // Test inventory
  Inventory* inventory = createTestInventory(itemList);

  // Initialize player data
  COISprite* playerSprite = COISpriteCreateFromAssetID(2240, 1984, 32, 32, loader, 1, COIWindowGetRenderer(window));
  PlayerInfo* pInfo = playerInfoCreate("Wique", playerSprite, inventory);
  actorFaceDown(pInfo->party[0]);

  COIBoard* titleBoard = titleCreateBoard();
  COIWindowSetBoard(COI_GLOBAL_WINDOW, titleBoard, title);
  
  COIBoard* townBoard = townCreateBoard(window, loader, pInfo);
  COILoop threadTownLoop = &threadTown;
  //COIWindowSetBoard(window, townBoard, threadTownLoop);
  COIWindowLoop(window);
  
  // Cleanup
  COIAssetLoaderDestroy(loader);
  COIBoardDestroy(townBoard);
  COIBoardDestroy(titleBoard);
  COIWindowDestroy(window);
  ItemListDestroy(itemList);
  inventoryDestroy(inventory);

  return 0;
}
