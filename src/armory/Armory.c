#include "Armory.h"

// Return the price for an item for this shop
static int _priceFromItemID(int item) {
  switch (item) {
  case ITEM_ID_RUSTY_SWORD:
    return 210;
  case ITEM_ID_RUSTY_BATTLEAXE:
    return 415;
  case ITEM_ID_SHABBY_BOW:
    return 320;
  case ITEM_ID_CRACKED_SHIELD:
    return 85;
  case ITEM_ID_STRENGTH_POTION:
    return 25;
  case ITEM_ID_DEF_SCROLL:
    return 20;
  case ITEM_ID_AGI_SCROLL:
    return 15;
  case ITEM_ID_BRONZE_HELM:
    return 300;
  case ITEM_ID_BRONZE_CHEST:
    return 430;
  case ITEM_ID_BRONZE_LEGS:
    return 210;
  case ITEM_ID_GEM_OF_PERMANENCE:
    return 40;
  case ITEM_ID_TAGNESSE:
    return 900;
  case ITEM_ID_KNIFE:
    return 90;
  case ITEM_ID_HEALING_POTION:
    return 60;
  case ITEM_ID_TP_POTION:
    return 45;
  case ITEM_ID_SP_POTION:
    return 50;
  case ITEM_ID_MOUNTAIN_JUICE:
    return 100;
  case ITEM_ID_LEATHER_ARMOR:
    return 230;
  case ITEM_ID_BOOTS:
    return 200;
  case ITEM_ID_SLEEP_IN:
    return 3;
  case ITEM_ID_READ_BOOK:
    return 6;
  case ITEM_ID_MAKE_FRIEND:
    return 25;
  case ITEM_ID_TAKE_WALK:
    return 1;
  case ITEM_ID_EXERCISE:
    return 1;
  case ITEM_ID_RUN_ERRANDS:
    return 2;
  case ITEM_ID_COOK:
    return 1;
  case ITEM_ID_GARDEN:
    return 4;
  case ITEM_ID_DATE:
    return 40;
  case ITEM_ID_NEW_HOBBY:
    return 30;
  default:
    printf("Error: No valid text ID %i\n", item);
    return -1;
  }
}


static ArmoryContext* _armoryCreateContext(COIBoard* board,
				    COIBoard* outsideBoard,
				    COIWindow* window,
				    PlayerInfo* pInfo,
				    IntList* itemIDs,
            bool isGag) {

  ArmoryContext* armoryContext = malloc(sizeof(ArmoryContext));

  armoryContext->pInfo = pInfo;
  armoryContext->isGag = isGag;
  armoryContext->sellItems = NULL;
  armoryContext->buyItems = NULL;
  armoryContext->board = board;

  armoryContext->textType = COITextTypeCreate(16, 255, 255, 255, COIWindowGetRenderer(window));
  armoryContext->mainStrings[0] = COIStringCreate("Buy", 0, 0, armoryContext->textType);
  armoryContext->mainStrings[1] = COIStringCreate("Sell", 0, 0, armoryContext->textType);
  armoryContext->mainStrings[2] = COIStringCreate("Exit", 0, 0, armoryContext->textType);
  
  // First level menu for armory
  COISprite* frame = COISpriteCreateFromAssetID(60, 40, 200, 190, COI_GLOBAL_LOADER, 5, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COISprite* pointer = COISpriteCreateFromAssetID(0, 0, 32, 32, COI_GLOBAL_LOADER, 6, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COIBoardAddDynamicSprite(board, frame, 0);
  COIBoardAddDynamicSprite(board, pointer, 0);
  COIMenu* menu = COIMenuCreate(frame, pointer);
  COIMenuSetTexts(menu, armoryContext->mainStrings, 3);
  COIMenuSetVisible(menu);
  armoryContext->menu = menu;

  // Fill item list for buy menu
  frame = COISpriteCreateFromAssetID(280, 40, 350, 190, COI_GLOBAL_LOADER, 5, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  pointer = COISpriteCreateFromAssetID(0, 0, 32, 32, COI_GLOBAL_LOADER, 6, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COIBoardAddDynamicSprite(board, frame, 0);
  COIBoardAddDynamicSprite(board, pointer, 0);
  armoryContext->buyMenu = COIMenuCreate(frame, pointer);
  armoryPopulateBuy(armoryContext, itemIDs);
  armoryUpdateMenuText(armoryContext->buyMenu, armoryContext->buyItems, armoryContext->numBuyItems);
  COIMenuSetInvisible(armoryContext->buyMenu);
  
  // Fill item list for sell menu
  armoryContext->sellMenu = COIMenuCreate(frame, pointer);
  armoryContext->inventory = pInfo->inventory;
  armoryPopulateSell(armoryContext);
  COIMenuSetInvisible(armoryContext->sellMenu);

  // Menu that asks for confirmation when buying/selling
  frame = COISpriteCreateFromAssetID(280, 250, 100, 70, COI_GLOBAL_LOADER, 5, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  pointer = COISpriteCreateFromAssetID(0, 0, 32, 32, COI_GLOBAL_LOADER, 6, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COIBoardAddDynamicSprite(board, frame, 0);
  COIBoardAddDynamicSprite(board, pointer, 0);
  armoryContext->confirmMenu = COIMenuCreate(frame, pointer);
  armoryContext->confirmStrings[0] = COIStringCreate("No", 0, 0, armoryContext->textType);
  armoryContext->confirmStrings[1] = COIStringCreate("Yes", 0, 0, armoryContext->textType);
  COIMenuSetTexts(armoryContext->confirmMenu, armoryContext->confirmStrings, 2);
  COIMenuSetInvisible(armoryContext->confirmMenu);
  armoryContext->confirmActive = false;

  // Display money
  if (armoryContext->isGag) {
    frame = COISpriteCreateFromAssetID(60, 245, 285, 50, COI_GLOBAL_LOADER, 5, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  } else {
    frame = COISpriteCreateFromAssetID(60, 245, 200, 50, COI_GLOBAL_LOADER, 5, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  }
  COIBoardAddDynamicSprite(board, frame, 0);
  armoryContext->moneyString = NULL;
  armoryUpdateMoneyString(armoryContext);

  armoryContext->outsideBoard = outsideBoard;
  armoryContext->currentMenu = menu;
  

  return armoryContext;
}

void armorySetItem(ArmoryContext* context, ArmoryItem* item, int itemID, int stock, bool sell, int slot) {
  item->itemID = itemID;
  if (item->string != NULL) {
    COIBoardRemoveString(context->board, item->string, 0);
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
  COIBoardAddString(context->board, item->string, 0);
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
  // COIString* strings[6 + context->numBuyItems + context->numSellItems];
  for (int i = 0; i < 3; i++) {
    // strings[i] = context->mainStrings[i];
    COIBoardAddString(board, context->mainStrings[i], 0);
  }
  for (int i = 0; i < 2; i++) {
    // strings[3+i] = context->confirmStrings[i];
    COIBoardAddString(board, context->confirmStrings[i], 0);
  }
  for (int i = 0; i < context->numBuyItems; i++) {
    // strings[5+i] = context->buyItems[i].string;
    COIBoardAddString(board, context->buyItems[i].string, 0);
  }
  for (int i = 0; i < context->numSellItems; i++) {
    // strings[5+context->numBuyItems+i] = context->sellItems[i].string;
    COIBoardAddString(board, context->sellItems[i].string, 0);
  }

  printf("string count: %i\n", board->drawLayers[0].stringCount);
  // strings[5+context->numBuyItems+context->numSellItems] = context->moneyString;

  
  // COIBoardSetStrings(board, strings, 6 + context->numBuyItems + context->numSellItems);
}

// Read money value from Inventory and update COIString
void armoryUpdateMoneyString(ArmoryContext* context) {
  if (context->moneyString != NULL) {
    COIBoardRemoveString(context->board, context->moneyString, 0);
    COIStringDestroy(context->moneyString);
  }

  char buf[MAX_STRING_SIZE];
  if (context->isGag) {
    sprintf(buf, "Hours Worked: %u", context->pInfo->shiftsWorked * 12);
  } else {
    sprintf(buf, "Gold: %i", context->inventory->money);
  }
  // Hardcoded, but we can base it off the box's position in the future
  context->moneyString = COIStringCreate(buf, 70, 250 + COI_MENU_OFFSET_Y, context->textType);
  COIBoardAddString(context->board, context->moneyString, 0);
}

// Initalize the "sell" menu items from items in the player's inventory
void armoryPopulateSell(ArmoryContext* context) {
  Inventory* inventory = context->inventory;

  if(context->sellItems != NULL) {
    for (int i = 0; i < context->numSellItems; i++) {
      COIBoardRemoveString(context->board, context->sellItems[i].string, 0);
      COIStringDestroy(context->sellItems[i].string);
    }
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
    armoryPopulateSell(context); // Because we've just added something to our inventory
    armoryUpdateMoneyString(context);
    COIMenuSetInvisible(context->sellMenu);
    COIMenuReset(context->buyMenu);
    COIMenuSetVisible(context->buyMenu);
    // armoryUpdateBoardText(board);

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
    successful = inventoryRemoveEquippedItem(context->inventory, item.slot);
  } else {
    // Item is in backpack
    successful = inventoryRemoveBackpackItem(context->inventory, itemIndex);
  }
  if (successful) {
    context->inventory->money = MIN(MAX_MONEY, context->inventory->money + item.price);
    for (int i = 0; i < context->sellMenu->_stringCount; i++) {
      COIBoardRemoveString(board, context->sellMenu->_strings[i], 0);
    }
    armoryPopulateSell(context);
    armoryUpdateMoneyString(context);
    COIMenuSetInvisible(context->buyMenu);
    COIMenuReset(context->sellMenu);
    COIMenuSetVisible(context->sellMenu);
    // armoryUpdateBoardText(board);

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
			    PlayerInfo* pInfo,
			    IntList* itemIDs,
          bool isGag) {
  
  COIBoard* armoryBoard = COIBoardCreate(99, 91, 95, 255, 640, 480, loader);

  COISprite* background = COISpriteCreateFromAssetID(0, 0, 640, 480, COI_GLOBAL_LOADER, 92, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COIBoardAddDynamicSprite(armoryBoard, background, 0);

  COILoop armoryLoop = &armory;

  ArmoryContext* context = _armoryCreateContext(armoryBoard, outsideBoard, window, pInfo, itemIDs, isGag);
  COIBoardSetContext(armoryBoard, (void*)context);

  armoryUpdateBoardText(armoryBoard);

  return armoryBoard;
}

COIBoard* _armoryCreateBoardForGag(COIBoard* outsideBoard, PlayerInfo* pInfo) {
  Inventory* inventory = pInfo->inventory;
  IntList itemIDs;

  IntListInitialize(&itemIDs, 10);
  IntListAdd(&itemIDs, ITEM_ID_SLEEP_IN);
  IntListAdd(&itemIDs, ITEM_ID_READ_BOOK);
  IntListAdd(&itemIDs, ITEM_ID_MAKE_FRIEND);
  IntListAdd(&itemIDs, ITEM_ID_TAKE_WALK);
  IntListAdd(&itemIDs, ITEM_ID_EXERCISE);
  IntListAdd(&itemIDs, ITEM_ID_RUN_ERRANDS);
  IntListAdd(&itemIDs, ITEM_ID_COOK);
  IntListAdd(&itemIDs, ITEM_ID_GARDEN);
  IntListAdd(&itemIDs, ITEM_ID_DATE);
  IntListAdd(&itemIDs, ITEM_ID_NEW_HOBBY);

  pInfo->armoryGagUsed = true;

  return armoryCreateBoard(COI_GLOBAL_WINDOW,
        COI_GLOBAL_LOADER,
        outsideBoard,
        pInfo,
        &itemIDs,
        true);
}

static bool _shouldDoGag(PlayerInfo* pInfo) {
  return !pInfo->armoryGagUsed && pInfo->shiftsWorked >= CLERK_CLASS_CHANGE_SHIFTS_WORKED;
}

COIBoard* armoryCreateBoardForWeaponsStore(COIBoard* outsideBoard, PlayerInfo* pInfo) {
  if (_shouldDoGag(pInfo)) {
    return _armoryCreateBoardForGag(outsideBoard, pInfo);
  }

  Inventory* inventory = pInfo->inventory;
  IntList itemIDs;

  switch (pInfo->class) {
    case PLAYER_CLASS_FIGHTER:
      IntListInitialize(&itemIDs, 10);
      IntListAdd(&itemIDs, ITEM_ID_RUSTY_SWORD);
      IntListAdd(&itemIDs, ITEM_ID_RUSTY_BATTLEAXE);
      IntListAdd(&itemIDs, ITEM_ID_SHABBY_BOW);
      IntListAdd(&itemIDs, ITEM_ID_KNIFE);
      IntListAdd(&itemIDs, ITEM_ID_CRACKED_SHIELD);
      IntListAdd(&itemIDs, ITEM_ID_BRONZE_HELM);
      IntListAdd(&itemIDs, ITEM_ID_LEATHER_ARMOR);
      IntListAdd(&itemIDs, ITEM_ID_BRONZE_CHEST);
      IntListAdd(&itemIDs, ITEM_ID_BOOTS);
      IntListAdd(&itemIDs, ITEM_ID_BRONZE_LEGS);
      break;
    case PLAYER_CLASS_ROGUE:
      IntListInitialize(&itemIDs, 6);
      IntListAdd(&itemIDs, ITEM_ID_RUSTY_SWORD);
      IntListAdd(&itemIDs, ITEM_ID_SHABBY_BOW);
      IntListAdd(&itemIDs, ITEM_ID_KNIFE);
      IntListAdd(&itemIDs, ITEM_ID_CRACKED_SHIELD);
      IntListAdd(&itemIDs, ITEM_ID_LEATHER_ARMOR);
      IntListAdd(&itemIDs, ITEM_ID_BOOTS);
      break;
    case PLAYER_CLASS_WIZARD:
      IntListInitialize(&itemIDs, 3);
      IntListAdd(&itemIDs, ITEM_ID_KNIFE);
      IntListAdd(&itemIDs, ITEM_ID_CRACKED_SHIELD);
      IntListAdd(&itemIDs, ITEM_ID_LEATHER_ARMOR);
      break;
    default:
      printf("Invalid class type in armory\n");
      IntListInitialize(&itemIDs, 1);
  }

  return armoryCreateBoard(COI_GLOBAL_WINDOW,
			   COI_GLOBAL_LOADER,
			   outsideBoard,
			   pInfo,
			   &itemIDs,
         false);
}

COIBoard* armoryCreateBoardForPotionStore(COIBoard* outsideBoard, PlayerInfo* pInfo) {
  if (_shouldDoGag(pInfo)) {
    return _armoryCreateBoardForGag(outsideBoard, pInfo);
  }

  IntList itemIDs;
  IntListInitialize(&itemIDs, 4);
  IntListAdd(&itemIDs, ITEM_ID_HEALING_POTION);
  IntListAdd(&itemIDs, ITEM_ID_TP_POTION);
  IntListAdd(&itemIDs, ITEM_ID_SP_POTION);
  IntListAdd(&itemIDs, ITEM_ID_MOUNTAIN_JUICE);

  return armoryCreateBoard(COI_GLOBAL_WINDOW,
			   COI_GLOBAL_LOADER,
			   outsideBoard,
			   pInfo,
			   &itemIDs,
         false);
}

COIBoard* armoryCreateBoardForGeneralStore(COIBoard* outsideBoard, PlayerInfo* pInfo) {
  if (_shouldDoGag(pInfo)) {
    return _armoryCreateBoardForGag(outsideBoard, pInfo);
  }

  IntList itemIDs;
  IntListInitialize(&itemIDs, 4);
  IntListAdd(&itemIDs, ITEM_ID_STRENGTH_POTION);
  IntListAdd(&itemIDs, ITEM_ID_DEF_SCROLL);
  IntListAdd(&itemIDs, ITEM_ID_AGI_SCROLL);
  IntListAdd(&itemIDs, ITEM_ID_GEM_OF_PERMANENCE);

  return armoryCreateBoard(COI_GLOBAL_WINDOW,
			   COI_GLOBAL_LOADER,
			   outsideBoard,
			   pInfo,
			   &itemIDs,
         false);
}

void armoryDestroy(ArmoryContext* context) {
  if (context->buyItems != NULL) {
    for (int i = 0; i < context->numBuyItems; i++) {
      COIBoardRemoveString(context->board, context->buyItems[i].string, 0);
      COIStringDestroy(context->buyItems[i].string);
    }
    free(context->buyItems);
  }
  if (context->sellItems != NULL) {
    for (int i = 0; i < context->numSellItems; i++) {
      COIBoardRemoveString(context->board, context->sellItems[i].string, 0);
      COIStringDestroy(context->sellItems[i].string);
    }
    free(context->sellItems);
  }
  COIBoardRemoveString(context->board, context->mainStrings[0], 0);
  COIBoardRemoveString(context->board, context->mainStrings[1], 0);
  COIBoardRemoveString(context->board, context->mainStrings[2], 0);
  COIBoardRemoveString(context->board, context->moneyString, 0);
  COIStringDestroy(context->mainStrings[0]);
  COIStringDestroy(context->mainStrings[1]);
  COIStringDestroy(context->mainStrings[2]);
  COIStringDestroy(context->moneyString);
  COIMenuDestroy(context->buyMenu);
  COIMenuDestroy(context->sellMenu);
  COIMenuDestroy(context->confirmMenu);
  free(context);
}


