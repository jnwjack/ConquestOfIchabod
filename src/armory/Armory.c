#include "Armory.h"

void armorySetItem(ArmoryContext* context, ArmoryItem* item, int itemID, int stock) {
  item->itemID = itemID;
  if (item->string != NULL) {
    COIStringDestroy(item->string);
  }
  item->string = COIStringCreate(_stringFromItemID(itemID), 0, 0, context->textType);
  item->stock = stock;
  item->price = _priceFromItemID(itemID);
}

// Update COIMenu text with items in ArmoryList
void armoryUpdateMenuText(COIMenu* menu, ArmoryItem* items, int numItems) {
  COIString* stringPointers[numItems];
  
  for (int i = 0; i < numItems; i++) {
    stringPointers[i] = items[i].string;
  }

  COIMenuSetTexts(menu, stringPointers, numItems);
}

// Send strings to board so the board can draw them
void armoryUpdateBoardText(COIBoard* board) {
  ArmoryContext* context = (ArmoryContext*)board->context;
  COIString* strings[3 + context->numBuyItems + context->numSellItems];
  for (int i = 0; i < 3; i++) {
    strings[i] = context->mainStrings[i];
  }
  for (int i = 0; i < context->numBuyItems; i++) {
    strings[3+i] = context->buyItems[i].string;
  }
  for (int i = 0; i < context->numSellItems; i++) {
    strings[3+context->numBuyItems+i] = context->sellItems[i].string;
  }
  COIBoardSetStrings(board, strings, 3 + context->numBuyItems + context->numSellItems);
}

// Initalize the "sell" menu items from items in the player's inventory
void armoryPopulateSell(ArmoryContext* context) {
  Inventory* inventory = context->inventory;
  
  context->numSellItems = inventory->numBackpackItems;
  context->sellItems = malloc(context->numSellItems * sizeof(ArmoryItem));

  for (int i = 0; i < inventory->numBackpackItems; i++) {
    context->sellItems[i].string = NULL;
    armorySetItem(context, &context->sellItems[i], inventory->backpack[i]->id, 1);
  }

  armoryUpdateMenuText(context->sellMenu, context->sellItems, context->numSellItems);
}

// Initialize the "buy" menu items
void armoryPopulateBuy(ArmoryContext* context) {
  context->numBuyItems = 5;
  context->buyItems = malloc(context->numBuyItems * sizeof(ArmoryItem));

  for (int i = 0; i < context->numBuyItems; i++) {
    context->buyItems[i].string = NULL;
  }
  
  // Hardcoded prices and stock values
  armorySetItem(context, &context->buyItems[0], ITEM_ID_RUSTY_SWORD, 1);
  armorySetItem(context, &context->buyItems[1], ITEM_ID_RUSTY_BATTLEAXE, 1);
  armorySetItem(context, &context->buyItems[2], ITEM_ID_SHABBY_BOW, 1);
  armorySetItem(context, &context->buyItems[3], ITEM_ID_CRACKED_SHIELD, 1);
  armorySetItem(context, &context->buyItems[4], ITEM_ID_STRENGTH_POTION, 5);

  armoryUpdateMenuText(context->buyMenu, context->buyItems, context->numBuyItems);
}

void armoryBuyMenu(ArmoryContext* context, int selected) {
}

// Initialization/Destruction ----------------------------------

COIBoard* armoryCreateBoard(COIWindow* window, COIAssetLoader* loader, COIBoard* outsideBoard, Inventory* inventory) {
  COIBoard* armoryBoard = COIBoardCreate(99, 91, 95, 255, 640, 480);

  COILoop armoryLoop = &armory;
  COIBoardLoadSpriteMap(armoryBoard, loader, COIWindowGetRenderer(window), "src/armory/spritemap.dat");

  COIBoardSetContext(armoryBoard, (void*)_armoryCreateContext(armoryBoard, outsideBoard, window, inventory));

  armoryUpdateBoardText(armoryBoard);

  return armoryBoard;
}

ArmoryContext* _armoryCreateContext(COIBoard* board, COIBoard* outsideBoard, COIWindow* window, Inventory* inventory) {
  ArmoryContext* armoryContext = malloc(sizeof(ArmoryContext));
  COISprite** armorySprites = COIBoardGetSprites(board);

  armoryContext->textType = COITextTypeCreate(25, 255, 255, 255, COIWindowGetRenderer(window));
  armoryContext->mainStrings[0] = COIStringCreate("Buy", 0, 0, armoryContext->textType);
  armoryContext->mainStrings[1] = COIStringCreate("Sell", 0, 0, armoryContext->textType);
  armoryContext->mainStrings[2] = COIStringCreate("Exit", 0, 0, armoryContext->textType);
  
  // First level menu for armory
  COIMenu* menu = COIMenuCreate(armorySprites[0], armorySprites[1]);
  COIMenuSetTexts(menu, armoryContext->mainStrings, 3);
  COIMenuSetVisible(menu);
  armoryContext->menu = menu;

  // Fill item list for buy menu
  armoryContext->buyMenu = COIMenuCreate(armorySprites[2], armorySprites[3]);
  armoryPopulateBuy(armoryContext);
  armoryUpdateMenuText(armoryContext->buyMenu, armoryContext->buyItems, armoryContext->numBuyItems);
  COIMenuSetInvisible(armoryContext->buyMenu);
  
  // Fill item list for sell menu
  armoryContext->sellMenu = COIMenuCreate(armorySprites[2], armorySprites[3]);
  armoryContext->inventory = inventory;
  armoryPopulateSell(armoryContext);
  COIMenuSetInvisible(armoryContext->sellMenu);

  armoryContext->outsideBoard = outsideBoard;
  armoryContext->currentMenu = menu;
  
  armoryContext->window = window;

  return armoryContext;
}

void armoryDestroy(ArmoryContext* context) {
  if (context->buyItems != NULL) {
    for (int i = 0; i < context->numBuyItems; i++) {
      COIStringDestroy(context->buyItems[i].string);
    }
    free(context->buyItems);
  }
  if (context->sellItems != NULL) {
    for (int i = 0; i < context->numSellItems; i++) {
      COIStringDestroy(context->sellItems[i].string);
    }
    free(context->sellItems);
  }
  COIStringDestroy(context->mainStrings[0]);
  COIStringDestroy(context->mainStrings[1]);
  COIStringDestroy(context->mainStrings[2]);
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

char* _stringFromItemID(int item) {
  return "test";
}
