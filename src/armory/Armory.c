#include "Armory.h"

void armorySetItem(ArmoryContext* context, ArmoryItem* item, int itemID, int stock, bool sell, int slot) {
  item->itemID = itemID;
  if (item->string != NULL) {
    COIStringDestroy(item->string);
  }
  item->stock = stock;
  item->price = sell ? _priceFromItemID(itemID) * SELL_FACTOR : _priceFromItemID(itemID);
  item->slot = slot;

  char buf[MAX_STRING_SIZE];
  if (slot != ITEM_SLOT_NA) {
    // This is an equipped item
    sprintf(buf, "%i - %s(E)", item->price, ItemListStringFromItemID(itemID));
  } else {
    sprintf(buf, "%i - %s", item->price, ItemListStringFromItemID(itemID));
  }
  item->string = COIStringCreate(buf, 0, 0, context->textType);
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
  COIString* strings[6 + context->numBuyItems + context->numSellItems];
  for (int i = 0; i < 3; i++) {
    strings[i] = context->mainStrings[i];
  }
  for (int i = 0; i < 2; i++) {
    strings[3+i] = context->confirmStrings[i];
  }
  for (int i = 0; i < context->numBuyItems; i++) {
    strings[5+i] = context->buyItems[i].string;
  }
  for (int i = 0; i < context->numSellItems; i++) {
    strings[5+context->numBuyItems+i] = context->sellItems[i].string;
  }
  strings[5+context->numBuyItems+context->numSellItems] = context->moneyString;
  
  COIBoardSetStrings(board, strings, 6 + context->numBuyItems + context->numSellItems);
}

// Read money value from Inventory and update COIString
void armoryUpdateMoneyString(ArmoryContext* context) {
  if (context->moneyString != NULL) {
    COIStringDestroy(context->moneyString);
  }
  char buf[MAX_STRING_SIZE];
  sprintf(buf, "Gold: %i", context->inventory->money);
  // Hardcoded, but we can base it off the box's position in the future
  context->moneyString = COIStringCreate(buf, 70, 250 + COI_MENU_OFFSET_Y, context->textType);
}

// Initalize the "sell" menu items from items in the player's inventory
void armoryPopulateSell(ArmoryContext* context) {
  Inventory* inventory = context->inventory;

  if(context->sellItems != NULL) {
    free(context->sellItems);
  }
  context->numSellItems = inventory->numBackpackItems + inventory->numEquippedItems;
  context->sellItems = malloc(context->numSellItems * sizeof(ArmoryItem));

  for (int i = 0; i < inventory->numBackpackItems; i++) {
    context->sellItems[i].string = NULL;
    armorySetItem(context, &context->sellItems[i], inventory->backpack[i]->id, 1, true, ITEM_SLOT_NA);
  }

  // Equipped items
  Item** equipped = inventoryGetEquippedItems(inventory);
  for (int i = 0; i < inventory->numEquippedItems; i++) {
    int sellItemIndex = inventory->numBackpackItems + i;
    context->sellItems[sellItemIndex].string = NULL;
    armorySetItem(context, &context->sellItems[sellItemIndex], equipped[i]->id, 1, true, equipped[i]->slot);
  }
  free(equipped);

  armoryUpdateMenuText(context->sellMenu, context->sellItems, context->numSellItems);
}

// Initialize the "buy" menu items
void armoryPopulateBuy(ArmoryContext* context, IntList* itemIDs) {
  if(context->sellItems != NULL) {
    free(context->sellItems);
  }

  context->numBuyItems = itemIDs->length;
  //context->numBuyItems = 8;
  context->buyItems = malloc(context->numBuyItems * sizeof(ArmoryItem));

  for (int i = 0; i < context->numBuyItems; i++) {
    context->buyItems[i].string = NULL;
    armorySetItem(context, &context->buyItems[i], itemIDs->values[i], 1, false, ITEM_SLOT_NA);
  }

  
  // Hardcoded prices and stock values
  /*
  armorySetItem(context, &context->buyItems[0], ITEM_ID_RUSTY_SWORD, 1, false, ITEM_SLOT_NA);
  armorySetItem(context, &context->buyItems[1], ITEM_ID_RUSTY_BATTLEAXE, 1, false, ITEM_SLOT_NA);
  armorySetItem(context, &context->buyItems[2], ITEM_ID_SHABBY_BOW, 1, false, ITEM_SLOT_NA);
  armorySetItem(context, &context->buyItems[3], ITEM_ID_CRACKED_SHIELD, 1, false, ITEM_SLOT_NA);
  armorySetItem(context, &context->buyItems[4], ITEM_ID_STRENGTH_POTION, 5, false, ITEM_SLOT_NA);
  armorySetItem(context, &context->buyItems[5], ITEM_ID_BRONZE_HELM, 1, false, ITEM_SLOT_NA);
  armorySetItem(context, &context->buyItems[6], ITEM_ID_BRONZE_CHEST, 1, false, ITEM_SLOT_NA);
  armorySetItem(context, &context->buyItems[7], ITEM_ID_BRONZE_LEGS, 1, false, ITEM_SLOT_NA);
  */

  armoryUpdateMenuText(context->buyMenu, context->buyItems, context->numBuyItems);
}

void armoryBuyItem(COIBoard* board) {
  ArmoryContext* context = (ArmoryContext*)board->context;
  ArmoryItem item = context->buyItems[context->buyMenu->_current];
  if (context->inventory->money >= item.price && inventoryAddItem(context->inventory, item.itemID)) {
    context->inventory->money -= item.price;
    armoryPopulateSell(context);
    armoryUpdateMoneyString(context);
    COIMenuSetInvisible(context->sellMenu);
    COIMenuReset(context->buyMenu);
    COIMenuSetVisible(context->buyMenu);
    armoryUpdateBoardText(board);

    // Mark down that we need to update our pause menu
    TownContext* townContext = (TownContext*)context->outsideBoard->context;
    townContext->pauseOverlay->dirty = true;
  }
  
  armoryDisableConfirmMenu(context);
}

void armorySellItem(COIBoard* board) {
  ArmoryContext* context = (ArmoryContext*)board->context;
  int itemIndex = context->sellMenu->_current;
  ArmoryItem item = context->sellItems[itemIndex];
  bool itemIsEquipped = item.slot != ITEM_SLOT_NA;
  bool successful;
  if (item.slot != ITEM_SLOT_NA) {
    // Item is equipped
    printf("trying to remove equip item...\n");
    successful = inventoryRemoveEquippedItem(context->inventory, item.slot);
  } else {
    // Item is in backpack
    successful = inventoryRemoveBackpackItem(context->inventory, itemIndex);
  }
  if (successful) {
    printf("removed equipped item!\n");
    context->inventory->money = MIN(MAX_MONEY, context->inventory->money + item.price);
    armoryPopulateSell(context);
    armoryUpdateMoneyString(context);
    COIMenuSetInvisible(context->buyMenu);
    COIMenuReset(context->sellMenu);
    COIMenuSetVisible(context->sellMenu);
    armoryUpdateBoardText(board);

    // Mark down that we need to update our pause menu
    TownContext* townContext = (TownContext*)context->outsideBoard->context;
    townContext->pauseOverlay->dirty = true;
  }
  armoryDisableConfirmMenu(context);
}

void armoryDisableConfirmMenu(ArmoryContext* context) {
  COIMenuReset(context->confirmMenu);
  COIMenuSetInvisible(context->confirmMenu);
  context->confirmActive = false;
}

void armoryEnableConfirmMenu(ArmoryContext* context) {
  COIMenuSetVisible(context->confirmMenu);
  context->confirmActive = true;
}

// Initialization/Destruction ----------------------------------

COIBoard* armoryCreateBoard(COIWindow* window,
			    COIAssetLoader* loader,
			    COIBoard* outsideBoard,
			    Inventory* inventory,
			    IntList* itemIDs) {
  
  COIBoard* armoryBoard = COIBoardCreate(99, 91, 95, 255, 640, 480, loader);

  COILoop armoryLoop = &armory;
  COIBoardLoadSpriteMap(armoryBoard, COIWindowGetRenderer(window), "src/armory/spritemap.dat");

  ArmoryContext* context = _armoryCreateContext(armoryBoard, outsideBoard, window, inventory, itemIDs);
  COIBoardSetContext(armoryBoard, (void*)context);

  armoryUpdateBoardText(armoryBoard);

  return armoryBoard;
}

COIBoard* armoryCreateBoardForWeaponsStore(COIBoard* outsideBoard, Inventory* inventory) {
  IntList itemIDs;
  IntListInitialize(&itemIDs, 7);
  IntListAdd(&itemIDs, ITEM_ID_RUSTY_SWORD);
  IntListAdd(&itemIDs, ITEM_ID_RUSTY_BATTLEAXE);
  IntListAdd(&itemIDs, ITEM_ID_SHABBY_BOW);
  IntListAdd(&itemIDs, ITEM_ID_KNIFE);
  IntListAdd(&itemIDs, ITEM_ID_CRACKED_SHIELD);
  IntListAdd(&itemIDs, ITEM_ID_BRONZE_HELM);
  IntListAdd(&itemIDs, ITEM_ID_BRONZE_CHEST);
  IntListAdd(&itemIDs, ITEM_ID_BRONZE_LEGS);

  return armoryCreateBoard(COI_GLOBAL_WINDOW,
			   COI_GLOBAL_LOADER,
			   outsideBoard,
			   inventory,
			   &itemIDs);
}

COIBoard* armoryCreateBoardForPotionStore(COIBoard* outsideBoard, Inventory* inventory) {
  IntList itemIDs;
  IntListInitialize(&itemIDs, 4);
  IntListAdd(&itemIDs, ITEM_ID_HEALING_POTION);
  IntListAdd(&itemIDs, ITEM_ID_TP_POTION);
  IntListAdd(&itemIDs, ITEM_ID_SP_POTION);
  IntListAdd(&itemIDs, ITEM_ID_MOUNTAIN_JUICE);

  return armoryCreateBoard(COI_GLOBAL_WINDOW,
			   COI_GLOBAL_LOADER,
			   outsideBoard,
			   inventory,
			   &itemIDs);
}

COIBoard* armoryCreateBoardForGeneralStore(COIBoard* outsideBoard, Inventory* inventory) {
  IntList itemIDs;
  IntListInitialize(&itemIDs, 4);
  IntListAdd(&itemIDs, ITEM_ID_STRENGTH_POTION);
  IntListAdd(&itemIDs, ITEM_ID_DEF_SCROLL);
  IntListAdd(&itemIDs, ITEM_ID_AGI_SCROLL);
  IntListAdd(&itemIDs, ITEM_ID_GEM_OF_PERMANENCE);

  return armoryCreateBoard(COI_GLOBAL_WINDOW,
			   COI_GLOBAL_LOADER,
			   outsideBoard,
			   inventory,
			   &itemIDs);
}

ArmoryContext* _armoryCreateContext(COIBoard* board,
				    COIBoard* outsideBoard,
				    COIWindow* window,
				    Inventory* inventory,
				    IntList* itemIDs) {

  ArmoryContext* armoryContext = malloc(sizeof(ArmoryContext));
  COISprite** armorySprites = COIBoardGetSprites(board);

  armoryContext->sellItems = NULL;
  armoryContext->buyItems = NULL;

  armoryContext->textType = COITextTypeCreate(16, 255, 255, 255, COIWindowGetRenderer(window));
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
  armoryPopulateBuy(armoryContext, itemIDs);
  armoryUpdateMenuText(armoryContext->buyMenu, armoryContext->buyItems, armoryContext->numBuyItems);
  COIMenuSetInvisible(armoryContext->buyMenu);
  
  // Fill item list for sell menu
  armoryContext->sellMenu = COIMenuCreate(armorySprites[2], armorySprites[3]);
  armoryContext->inventory = inventory;
  armoryPopulateSell(armoryContext);
  COIMenuSetInvisible(armoryContext->sellMenu);

  // Menu that asks for confirmation when buying/selling
  armoryContext->confirmMenu = COIMenuCreate(armorySprites[5], armorySprites[6]);
  armoryContext->confirmStrings[0] = COIStringCreate("No", 0, 0, armoryContext->textType);
  armoryContext->confirmStrings[1] = COIStringCreate("Yes", 0, 0, armoryContext->textType);
  COIMenuSetTexts(armoryContext->confirmMenu, armoryContext->confirmStrings, 2);
  COIMenuSetInvisible(armoryContext->confirmMenu);
  armoryContext->confirmActive = false;

  // Display money
  armoryContext->moneyString = NULL;
  armoryUpdateMoneyString(armoryContext);


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
  COIStringDestroy(context->moneyString);
  COIMenuDestroy(context->buyMenu);
  COIMenuDestroy(context->sellMenu);
  COIMenuDestroy(context->confirmMenu);
  free(context);
}

//  ------------------------------------------------------------

// Return the price for an item for this shop
int _priceFromItemID(int item) {
  switch (item) {
  case ITEM_ID_RUSTY_SWORD:
    return 20;
  case ITEM_ID_RUSTY_BATTLEAXE:
    return 28;
  case ITEM_ID_SHABBY_BOW:
    return 20;
  case ITEM_ID_CRACKED_SHIELD:
    return 10;
  case ITEM_ID_STRENGTH_POTION:
    return 25;
  case ITEM_ID_DEF_SCROLL:
    return 20;
  case ITEM_ID_AGI_SCROLL:
    return 15;
  case ITEM_ID_BRONZE_HELM:
    return 30;
  case ITEM_ID_BRONZE_CHEST:
    return 50;
  case ITEM_ID_BRONZE_LEGS:
    return 30;
  case ITEM_ID_GEM_OF_PERMANENCE:
    return 120;
  case ITEM_ID_TAGNESSE:
    return 700;
  case ITEM_ID_KNIFE:
    return 13;
  case ITEM_ID_HEALING_POTION:
    return 25;
  case ITEM_ID_TP_POTION:
    return 15;
  case ITEM_ID_SP_POTION:
    return 20;
  case ITEM_ID_MOUNTAIN_JUICE:
    return 60;
  default:
    printf("Error: No valid text ID %i\n", item);
    return -1;
  }
}
