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

static bool _shouldDoGag(PlayerInfo* pInfo) {
  return !pInfo->armoryGagUsed && pInfo->shiftsWorked >= CLERK_CLASS_CHANGE_SHIFTS_WORKED;
}

static void _setItem(ArmoryContext* context, ArmoryItem* item, int itemID, int stock, bool sell, int slot) {
  item->itemID = itemID;
  item->stock = stock;
  item->price = sell ? _priceFromItemID(itemID) * SELL_FACTOR : _priceFromItemID(itemID);
  item->slot = slot;
}

static COIString* _getItemString(ArmoryContext* context, ArmoryItem* item) {
  char buf[MAX_STRING_SIZE];
  if (item->slot != ITEM_SLOT_NA) {
    // This is an equipped item
    snprintf(buf, MAX_STRING_SIZE, "%i - %s(E)", item->price, ItemListStringFromItemID(item->itemID));
  } else {
    snprintf(buf, MAX_STRING_SIZE, "%i - %s", item->price, ItemListStringFromItemID(item->itemID));
  }

  return COIStringCreate(buf, 0, 0, context->textType);
}

static void _populateBuy(ArmoryContext* context, IntList* itemIDs) {
  if (context->buyMenu) {
    COIMenuDestroyAndFreeComponents(context->sellMenu, context->board);
  }
  COISprite* frame = COISpriteCreateFromAssetID(280, 40, 350, 190, COI_GLOBAL_LOADER, 5, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COISprite* pointer = COISpriteCreateFromAssetID(0, 0, 32, 32, COI_GLOBAL_LOADER, 6, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COIBoardAddDynamicSprite(context->board, frame, 0);
  COIBoardAddDynamicSprite(context->board, pointer, 0);
  context->buyMenu = COIMenuCreateWithCapacity(frame, pointer, ARMORY_MAX_BUY_ITEMS);

  context->numBuyItems = itemIDs->length;

  for (int i = 0; i < context->numBuyItems; i++) {
    printf("buyitem\n");
    _setItem(context, &context->buyItems[i], itemIDs->values[i], 1, false, ITEM_SLOT_NA);
    printf("buyitem a\n");
    COIString* string = _getItemString(context, &context->buyItems[i]);
    printf("buyitem b\n");
    COIBoardAddString(context->board, string, 0);
    printf("buyitem c\n");
    COIMenuAddString(context->buyMenu, string, i);
    printf("buyitem d\n");
  }
}

// Initalize the "sell" menu items from items in the player's inventory
void _populateSell(ArmoryContext* context) {
  Inventory* inventory = context->inventory;

  if (context->sellMenu) {
    COIMenuDestroyAndFreeComponents(context->sellMenu, context->board);
    printf("destroying sell menu\n");
  }
  COISprite* frame = COISpriteCreateFromAssetID(280, 40, 350, 190, COI_GLOBAL_LOADER, 5, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COISprite* pointer = COISpriteCreateFromAssetID(0, 0, 32, 32, COI_GLOBAL_LOADER, 6, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COIBoardAddDynamicSprite(context->board, frame, 0);
  COIBoardAddDynamicSprite(context->board, pointer, 0);
  context->sellMenu = COIMenuCreateWithCapacity(frame, pointer, ARMORY_MAX_SELL_ITEMS);

  // for (int i = 0; i < context->numSellItems; i++) {
  //   // printf("DESTROYING FIRST CHAR: %i\n", context->sellItems[i].string->fontSize);
  //   COIMenuRemoveString(context->sellMenu, i, context->board);
  //   context->sellItems[i].string = NULL;
  // }

  // for (int i = 0; i < 15; i++) {
  //   char temp[MAX_STRING_SIZE];
  //   snprintf(temp, MAX_STRING_SIZE, "Heyo - %i", i);
  //   COIString* string = COIStringCreate(temp, 0, 0, context->textType);
  //   COIBoardAddString(context->board, string, 0);
  //   COIMenuAddString(context->sellMenu, string, i);
  // }

  // context->numSellItems = 15;
  // return;

  context->numSellItems = inventory->numBackpackItems + inventory->numEquippedItems;
  printf("NUM SELL: %i\n", context->numSellItems);
  for (int i = 0; i < inventory->numBackpackItems; i++) {
    _setItem(context, &context->sellItems[i], inventory->backpack[i]->id, 1, true, ITEM_SLOT_NA);
    COIString* string = _getItemString(context, &context->sellItems[i]);
    COIBoardAddString(context->board, string, 0);
    COIMenuAddString(context->sellMenu, string, i);
  }

  // // Equipped items
  Item** equipped = inventoryGetEquippedItems(inventory);
  for (int i = 0; i < inventory->numEquippedItems; i++) {
    int sellItemIndex = inventory->numBackpackItems + i;
    _setItem(context, &context->sellItems[sellItemIndex], equipped[i]->id, 1, true, equipped[i]->slot);
    COIString* string = _getItemString(context, &context->sellItems[i]);
    COIBoardAddString(context->board, string, 0);
    COIMenuAddString(context->sellMenu, string, i);
  }
  free(equipped);

  // context->sellMenu->_current = 0;
  // COIMenuAdjustFrame(context->sellMenu);
}

// Read money value from Inventory and update COIString
static void _updateMoneyString(ArmoryContext* context) {
  if (context->moneyString != NULL) {
    COIBoardRemoveString(context->board, context->moneyString, 0);
    COIStringDestroy(context->moneyString);
  }

  char buf[MAX_STRING_SIZE];
  if (context->isGag) {
    snprintf(buf, MAX_STRING_SIZE, "Hours Worked: %u", context->pInfo->shiftsWorked * 12);
  } else {
    snprintf(buf, MAX_STRING_SIZE, "Gold: %i", context->inventory->money);
  }
  // Hardcoded, but we can base it off the box's position in the future
  context->moneyString = COIStringCreate(buf, 70, 250 + COI_MENU_OFFSET_Y, context->textType);
  COIBoardAddString(context->board, context->moneyString, 0);
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

COIBoard* _createBoard(COIWindow* window,
			    COIAssetLoader* loader,
			    COIBoard* outsideBoard,
			    PlayerInfo* pInfo,
			    IntList* itemIDs,
          bool isGag) {
  
  COIBoard* armoryBoard = COIBoardCreate(99, 91, 95, 255, 640, 480, loader);

  COISprite* background = COISpriteCreateFromAssetID(0, 0, 640, 480, COI_GLOBAL_LOADER, 92, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COIBoardAddDynamicSprite(armoryBoard, background, 0);

  ArmoryContext* armoryContext = malloc(sizeof(ArmoryContext));

  armoryContext->pInfo = pInfo;
  armoryContext->isGag = isGag;
  armoryContext->numBuyItems = 0;
  armoryContext->numSellItems = 0;
  armoryContext->board = armoryBoard;

  armoryContext->textType = COITextTypeCreate(16, 255, 255, 255, COIWindowGetRenderer(window));
  armoryContext->mainStrings[0] = COIStringCreate("Buy", 0, 0, armoryContext->textType);
  COIBoardAddString(armoryBoard, armoryContext->mainStrings[0], 0);
  armoryContext->mainStrings[1] = COIStringCreate("Sell", 0, 0, armoryContext->textType);
  COIBoardAddString(armoryBoard, armoryContext->mainStrings[1], 0);
  armoryContext->mainStrings[2] = COIStringCreate("Exit", 0, 0, armoryContext->textType);
  COIBoardAddString(armoryBoard, armoryContext->mainStrings[2], 0);

  // First level menu for armory
  COISprite* frame = COISpriteCreateFromAssetID(60, 40, 200, 190, COI_GLOBAL_LOADER, 5, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COISprite* pointer = COISpriteCreateFromAssetID(0, 0, 32, 32, COI_GLOBAL_LOADER, 6, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COIBoardAddDynamicSprite(armoryBoard, frame, 0);
  COIBoardAddDynamicSprite(armoryBoard, pointer, 0);
  COIMenu* menu = COIMenuCreateWithCapacity(frame, pointer, 5);
  for (int i = 0; i < 3; i++) {
    COIMenuAddString(menu, armoryContext->mainStrings[i], i);
  }
  COIMenuSetVisible(menu);
  armoryContext->menu = menu;

  printf("HA 1\n");

  // Fill item list for buy menu
  // frame = COISpriteCreateFromAssetID(280, 40, 350, 190, COI_GLOBAL_LOADER, 5, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  // pointer = COISpriteCreateFromAssetID(0, 0, 32, 32, COI_GLOBAL_LOADER, 6, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  // COIBoardAddDynamicSprite(armoryBoard, frame, 0);
  // COIBoardAddDynamicSprite(armoryBoard, pointer, 0);
  armoryContext->buyMenu = NULL;
  // armoryContext->buyMenu = COIMenuCreateWithCapacity(frame, pointer, ARMORY_MAX_BUY_ITEMS);
  printf("HA 2\n");
  _populateBuy(armoryContext, itemIDs);
  printf("HA 3\n");
  // armoryUpdateMenuText(armoryContext->buyMenu, armoryContext->buyItems, armoryContext->numBuyItems);
  COIMenuSetInvisible(armoryContext->buyMenu);

  // Fill item list for sell menu
  // frame = COISpriteCreateFromAssetID(280, 40, 350, 190, COI_GLOBAL_LOADER, 5, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  // pointer = COISpriteCreateFromAssetID(0, 0, 32, 32, COI_GLOBAL_LOADER, 6, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  // COIBoardAddDynamicSprite(armoryBoard, frame, 0);
  // COIBoardAddDynamicSprite(armoryBoard, pointer, 0);
  // armoryContext->sellMenu = COIMenuCreateWithCapacity(frame, pointer, ARMORY_MAX_SELL_ITEMS);

  armoryContext->sellMenu = NULL;
  armoryContext->inventory = pInfo->inventory;
  _populateSell(armoryContext);
  COIMenuSetInvisible(armoryContext->sellMenu);

  // Menu that asks for confirmation when buying/selling
  frame = COISpriteCreateFromAssetID(280, 250, 100, 70, COI_GLOBAL_LOADER, 5, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  pointer = COISpriteCreateFromAssetID(0, 0, 32, 32, COI_GLOBAL_LOADER, 6, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COIBoardAddDynamicSprite(armoryContext->board, frame, 0);
  COIBoardAddDynamicSprite(armoryContext->board, pointer, 0);
  armoryContext->confirmMenu = COIMenuCreateWithCapacity(frame, pointer, 5);
  armoryContext->confirmStrings[0] = COIStringCreate("No", 0, 0, armoryContext->textType);
  armoryContext->confirmStrings[1] = COIStringCreate("Yes", 0, 0, armoryContext->textType);
  COIBoardAddString(armoryBoard, armoryContext->confirmStrings[0], 0);
  COIBoardAddString(armoryBoard, armoryContext->confirmStrings[1], 0);
  COIMenuAddString(armoryContext->confirmMenu, armoryContext->confirmStrings[0], 0);
  COIMenuAddString(armoryContext->confirmMenu, armoryContext->confirmStrings[1], 1);
  COIMenuSetInvisible(armoryContext->confirmMenu);
  armoryContext->confirmActive = false;

  // Display money
  if (armoryContext->isGag) {
    frame = COISpriteCreateFromAssetID(60, 245, 285, 50, COI_GLOBAL_LOADER, 5, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  } else {
    frame = COISpriteCreateFromAssetID(60, 245, 200, 50, COI_GLOBAL_LOADER, 5, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  }
  COIBoardAddDynamicSprite(armoryContext->board, frame, 0);
  armoryContext->moneyString = NULL;
  _updateMoneyString(armoryContext);

  armoryContext->outsideBoard = outsideBoard;
  armoryContext->currentMenu = menu;

  COIBoardSetContext(armoryBoard, (void*)armoryContext);

  return armoryBoard;
}

static COIBoard* _armoryCreateBoardForGag(COIBoard* outsideBoard, PlayerInfo* pInfo) {
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

  return _createBoard(COI_GLOBAL_WINDOW,
        COI_GLOBAL_LOADER,
        outsideBoard,
        pInfo,
        &itemIDs,
        true);
}

void armoryBuyItem(ArmoryContext* context) {
  ArmoryItem item = context->buyItems[COIMenuGetCurrentValue(context->buyMenu)];
  if (context->inventory->money >= item.price && inventoryAddItem(context->inventory, item.itemID)) {
    context->inventory->money -= item.price;
    _populateSell(context); // Because we've just added something to our inventory
    _updateMoneyString(context);
    COIMenuSetInvisible(context->sellMenu);
    // COIMenuReset(context->buyMenu);
    // COIMenuSetVisible(context->buyMenu);
    // // armoryUpdateBoardText(board);

    // Mark down that we need to update our pause menu
    TownContext* townContext = (TownContext*)context->outsideBoard->context;
    townContext->pauseOverlay->dirty = true;
  }
  
  armoryDisableConfirmMenu(context);
}

void armorySellItem(ArmoryContext* context) {
  // ArmoryContext* context = (ArmoryContext*)board->context;
  // _populateSell(context);
  int itemIndex = COIMenuGetCurrentValue(context->sellMenu);
  printf("ITEM INDEX: %i\n", itemIndex);
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
  successful = true;
  if (successful) {
    context->inventory->money = MIN(MAX_MONEY, context->inventory->money + item.price);
    _populateSell(context);
    _updateMoneyString(context);
    COIMenuSetInvisible(context->sellMenu);
    COIMenuReset(context->sellMenu);

    // Mark down that we need to update our pause menu
    TownContext* townContext = (TownContext*)context->outsideBoard->context;
    townContext->pauseOverlay->dirty = true;
  }
  armoryDisableConfirmMenu(context);
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

  return _createBoard(COI_GLOBAL_WINDOW,
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

  return _createBoard(COI_GLOBAL_WINDOW,
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

  return _createBoard(COI_GLOBAL_WINDOW,
			   COI_GLOBAL_LOADER,
			   outsideBoard,
			   pInfo,
			   &itemIDs,
         false);
}

void armoryProcessDirectionalInput(ArmoryContext* context, int direction) {
  COIMenu* focusedMenu = context->confirmActive ? context->confirmMenu : context->currentMenu;
  COIMenuHandleInput(focusedMenu, direction, false);
}

void armoryBack(ArmoryContext* context) {
  if (context->currentMenu != context->menu) {
    if (context->confirmActive) {
      COIMenuSetInvisible(context->confirmMenu);
    } else {
      COIMenuSetInvisible(context->currentMenu);
      COIMenuReset(context->currentMenu);
      context->currentMenu = context->menu;
    }
  }
}

// Returns true if we leave the armory
bool armorySelect(ArmoryContext* context) {
  if (context->confirmActive) {
    int val = COIMenuGetCurrentValue(context->confirmMenu);
    if (val == 0) {
      armoryDisableConfirmMenu(context);
    } else {
      if (COIMenuGetCurrentValue(context->menu) == 0) {
        armoryBuyItem(context);
      } else {
        armorySellItem(context);
        context->currentMenu = context->menu;
        COIMenuSetInvisible(context->sellMenu);
      }
    }
  } else {
    if (context->currentMenu == context->menu) {
      if (COIMenuGetCurrentValue(context->currentMenu) == 0) {
        COIMenuSetVisible(context->buyMenu);
        context->currentMenu = context->buyMenu;
      } else if (COIMenuGetCurrentValue(context->currentMenu) == 1) {
        if (context->numSellItems > 0) {
          COIMenuSetVisible(context->sellMenu);
          context->currentMenu = context->sellMenu;
        }
      } else {
        return true;
      }
    } else {
      armoryEnableConfirmMenu(context);
    }
  }

  return false;
}

void armoryDestroy(ArmoryContext* context) {
  COIBoardRemoveString(context->board, context->mainStrings[0], 0);
  COIBoardRemoveString(context->board, context->mainStrings[1], 0);
  COIBoardRemoveString(context->board, context->mainStrings[2], 0);
  COIBoardRemoveString(context->board, context->moneyString, 0);
  COIBoardRemoveString(context->board, context->confirmStrings[0], 0);
  COIBoardRemoveString(context->board, context->confirmStrings[1], 1);
  COIStringDestroy(context->mainStrings[0]);
  COIStringDestroy(context->mainStrings[1]);
  COIStringDestroy(context->mainStrings[2]);
  COIStringDestroy(context->moneyString);
  COIStringDestroy(context->confirmStrings[0]);
  COIStringDestroy(context->confirmStrings[1]);
  COIMenuDestroy(context->buyMenu);
  COIMenuDestroy(context->sellMenu);
  COIMenuDestroy(context->confirmMenu);
  free(context);
}
