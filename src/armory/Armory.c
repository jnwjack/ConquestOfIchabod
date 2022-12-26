#include "Armory.h"

void armorySetItem(ArmoryItem* item, int itemID, int stock) {
  item->textID = _textIDFromItemID(itemID);
  item->itemID = itemID;
  item->stock = stock;
  item->price = _priceFromItemID(itemID);
}

// Update COIMenu text with items in ArmoryList
void armoryUpdateMenuText(COIMenu* menu, ArmoryItem* items, int numItems) {
  int indices[numItems];
  for (int i = 0; i < numItems; i++) {
    indices[i] = items[i].textID;
  }

  COIMenuSetTexts(menu, indices, numItems);
}
  
// Traverse array of items and get text indices
int* armoryGetTextIndices(ArmoryContext* context) {
  int* textIndices = malloc(sizeof(int) * context->numBuyItems);
  
  int i;
  for (i = 0; i < context->numBuyItems; i++) {
    textIndices[i] = context->buyItems[i].textID;
  }

  return textIndices;
}

void armorySetTextIndices(ArmoryContext* context, int* indices) {
  for (int i = 0; i < context->numBuyItems; i++) {
    context->buyItems[i].textID = indices[i];
  }
}

// Initalize the "sell" menu items from items in the player's inventory
void armoryPopulateSell(ArmoryContext* context, Inventory* inventory) {
  context->numSellItems = inventory->numBackpackItems;
  context->sellItems = malloc(context->numSellItems * sizeof(ArmoryItem));

  for (int i = 0; i < inventory->numBackpackItems; i++) {
    armorySetItem(&context->sellItems[i], inventory->backpack[i]->id, 1);
  }

  armoryUpdateMenuText(context->sellMenu, context->sellItems, context->numSellItems);
}

// Initialize the "buy" menu items
void armoryPopulateBuy(ArmoryContext* context) {
  context->numBuyItems = 5;
  context->buyItems = malloc(context->numBuyItems * sizeof(ArmoryItem));

  
  // Hardcoded prices and stock values
  armorySetItem(&context->buyItems[0], ITEM_ID_RUSTY_SWORD, 1);
  armorySetItem(&context->buyItems[1], ITEM_ID_RUSTY_BATTLEAXE, 1);
  armorySetItem(&context->buyItems[2], ITEM_ID_SHABBY_BOW, 1);
  armorySetItem(&context->buyItems[3], ITEM_ID_CRACKED_SHIELD, 1);
  armorySetItem(&context->buyItems[4], ITEM_ID_STRENGTH_POTION, 5);

  armoryUpdateMenuText(context->buyMenu, context->buyItems, context->numBuyItems);
}

void armoryBuyMenu(ArmoryContext* context, int selected) {
}

// Initialization/Destruction ----------------------------------

COIBoard* armoryCreateBoard(COIWindow* window, COIAssetLoader* loader, COIBoard* outsideBoard) {
  COITextGroup* mediumGroup = COITextGroupCreate(25, 255, 255, 255, "src/armory/text.dat", COIWindowGetRenderer(window));
  COIBoard* armoryBoard = COIBoardCreate(99, 91, 95, 255, 640, 480, 1);
  COIBoardAddTextGroup(armoryBoard, mediumGroup, 0);

  COILoop armoryLoop = &armory;
  COIBoardLoadSpriteMap(armoryBoard, loader, COIWindowGetRenderer(window), "src/armory/spritemap.dat");

  COIBoardSetContext(armoryBoard, (void*)_armoryCreateContext(armoryBoard, outsideBoard, window, mediumGroup));
}
ArmoryContext* _armoryCreateContext(COIBoard* board, COIBoard* outsideBoard, COIWindow* window, COITextGroup* textGroup) {
  ArmoryContext* armoryContext = malloc(sizeof(ArmoryContext));
  COISprite** armorySprites = COIBoardGetSprites(board);

  // First level menu for armory
  COIMenu* menu = COIMenuCreate(textGroup, armorySprites[0], armorySprites[1]);
  int indices[3] = { TEXT_ID_BUY, TEXT_ID_SELL, TEXT_ID_EXIT };
  COIMenuSetTexts(menu, indices, 3);
  COIMenuSetVisible(menu);
  armoryContext->menu = menu;

  // Fill item list for buy menu
  armoryContext->buyMenu = COIMenuCreate(textGroup, armorySprites[2], armorySprites[3]);
  armoryPopulateBuy(armoryContext);
  armoryUpdateMenuText(armoryContext->buyMenu, armoryContext->buyItems, armoryContext->numBuyItems);
  COIMenuSetInvisible(armoryContext->buyMenu);
  armoryContext->sellItems = NULL;
  armoryContext->outsideBoard = outsideBoard;
  armoryContext->currentMenu = menu;

  armoryContext->window = window;

  return armoryContext;
}

void armoryDestroy(ArmoryContext* context) {
  if (context->buyItems != NULL) {
    free(context->buyItems);
  }
  if (context->sellItems != NULL) {
    free(context->sellItems);
  }
  free(context);
}

//  ------------------------------------------------------------

// Return the price for an item for this shop
int _priceFromItemID(int item) {
  switch (item) {
  case ITEM_ID_RUSTY_SWORD:
    return 20;
    break;
  case ITEM_ID_RUSTY_BATTLEAXE:
    return 28;
    break;
  case ITEM_ID_SHABBY_BOW:
    return 20;
    break;
  case ITEM_ID_CRACKED_SHIELD:
    return 10;
    break;
  case ITEM_ID_STRENGTH_POTION:
    return 10;
    break;
  default:
    printf("Error: No valid text ID\n");
    return -1;
  }
}

// Given an item ID, return the proper text as defined in armory/text.dat
// We may want to re-implement this in a more global way (e.g. text.dat for all items)
int _textIDFromItemID(int item) {
  switch (item) {
  case ITEM_ID_RUSTY_SWORD:
    return 3;
    break;
  case ITEM_ID_RUSTY_BATTLEAXE:
    return 4;
    break;
  case ITEM_ID_SHABBY_BOW:
    return 5;
    break;
  case ITEM_ID_CRACKED_SHIELD:
    return 6;
    break;
  case ITEM_ID_STRENGTH_POTION:
    return 7;
    break;
  default:
    printf("Error: No valid text ID\n");
    return -1;
  }
}
