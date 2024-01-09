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
  // Initialize global window
  COIWindowInit();
  
  // In the future, we can remove COIWindow member variables from structs so
  // we don't have to pass it around all over the place.
  COIWindow* window = COI_GLOBAL_WINDOW;
  
  COIAssetLoader* loader = COIAssetLoaderCreate();

  // Global item data
  ItemList* itemList = loadItems();

  // Test inventory
  Inventory* inventory = createTestInventory(itemList);

  // Initialize player data
  COISprite* playerSprite = COISpriteCreateFromAssetID(1216, 1472, 32, 32, loader, 1, COIWindowGetRenderer(window));
  PlayerInfo* pInfo = playerInfoCreate("Wique", playerSprite, inventory);

  
  COIBoard* townBoard = townCreateBoard(window, loader, pInfo);
  COILoop threadTownLoop = &threadTown;
  COIWindowSetBoard(window, townBoard, threadTownLoop);
  COIWindowLoop(window);

  // Cleanup
  COIAssetLoaderDestroy(loader);
  COIBoardDestroy(townBoard);
  COIWindowDestroy(window);
  ItemListDestroy(itemList);
  inventoryDestroy(inventory);

  return 0;
}
