// Should have a better system for includes
#include "engine/COIWindow.h"
#include "engine/COIMenu.h"
#include "inputloops.h"
#include "armory/Armory.h"


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
  COISprite** armorySprites = COIBoardGetSprites(armoryBoard);

  // First level menu for armory
  COIMenu* menu = COIMenuCreate(mediumGroup, armorySprites[0], armorySprites[1]);
  //int indices[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
  int indices[3] = { 0, 1, 2 };
  COIMenuSetTexts(menu, indices, 3);
  COIMenuSetVisible(menu);

  // Second level menu for armory
  int subMenuIndices[5] = { 3, 4, 5, 6, 7 };
  COIMenu* subMenu = COIMenuCreate(mediumGroup, armorySprites[2], armorySprites[3]);
  COIMenuSetTexts(subMenu, subMenuIndices, 5);
  COIMenuSetInvisible(subMenu);
  

  // Modify threadtown context
  COIBoard** armoryBoardPtr = (COIBoard**) (context + sizeof(int));
  *armoryBoardPtr = armoryBoard;
  COIWindow** windowPtr = (COIWindow**) (context + sizeof(int) + sizeof(COIBoard*));
  *windowPtr = window;
  COIBoardSetContext(board, context);

  ArmoryContext* armoryContext = malloc(sizeof(ArmoryContext));
  armoryContext->pointerLocation = 0;
  armoryContext->board = board;
  armoryContext->window = window;
  armoryContext->menu = menu;
  armoryContext->buyMenu = subMenu;
  armoryContext->currentMenu = menu;
  armoryContext->numItems = 5;
  for (int i = 0; i < armoryContext->numItems; i++) {
    //    armoryContext->items[i].textIndex = 
  }
  
  COIBoardSetContext(armoryBoard, (void*)armoryContext);

  
  COIWindowSetBoard(window, board, threadTownLoop);
  COIWindowLoop(window);


  // Cleanup
  // Only need to free the int, other pointers in context get handled elsewhere
  int* contextIntPtr = (int*) (context);
  free(contextIntPtr);
  /*contextIntPtr = (int*) (armoryContext);
    free(contextIntPtr);*/
  free(armoryContext);
  
  COITextGroupDestroy(mediumGroup);
  COIAssetLoaderDestroy(loader);
  COIBoardDestroy(board);
  COIBoardDestroy(armoryBoard);
  COIMenuDestroy(menu);
  COIMenuDestroy(subMenu);
  COIWindowDestroy(window);

  return 0;
}
