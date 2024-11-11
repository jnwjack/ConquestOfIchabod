#ifndef ARMORY_H
#define ARMORY_H

#include "../player.h"
#include "../inputloops.h"

// Can later replace this with charisma or something
#define SELL_FACTOR 0.75

typedef struct ArmoryItem {
  int itemID;
  int slot;
  COIString* string;
  int stock;
  int price;
} ArmoryItem;

// Set of indices to texts in file that indicate the armory's stock.
// there should be a armory object that holds current stock, prices, mappings to text object, etc.
typedef struct ArmoryContext {
  COIWindow* window;
  COIBoard* outsideBoard; // Board to load when leaving shop
  COIMenu* menu;
  // may want to wrap submenu in a "buymenu" object or something
  COIMenu* buyMenu;
  COIMenu* sellMenu;
  COIMenu* currentMenu;
  COIMenu* confirmMenu;
  bool confirmActive;
  // List of items available for buying/selling in shop
  ArmoryItem* buyItems;
  ArmoryItem* sellItems;
  int numBuyItems;
  int numSellItems;
  Inventory* inventory; // Player inventory
  COITextType* textType;
  COIString* mainStrings[3];
  COIString* confirmStrings[2];
  COIString* moneyString;
} ArmoryContext;

// Create board and associated context
// main.c can access context through the returned board
COIBoard* armoryCreateBoard(COIWindow* window,
			    COIAssetLoader* loader,
			    COIBoard* outsideBoard,
			    Inventory* inventory,
			    IntList* itemIDs);
COIBoard* armoryCreateBoardForWeaponsStore(COIBoard* outsideBoard, PlayerInfo* pInfo);
COIBoard* armoryCreateBoardForGeneralStore(COIBoard* outsideBoard, Inventory* inventory);
COIBoard* armoryCreateBoardForPotionStore(COIBoard* outsideBoard, Inventory* inventory);

void armoryDestroy(ArmoryContext* context);

void armorySetItem(ArmoryContext* context, ArmoryItem* item, int itemID, int stock, bool sell, int slot);
int* armoryGetTextIndices(ArmoryContext* context);
void armorySetTextIndices(ArmoryContext* context, int* indices);
void armoryPopulateBuy(ArmoryContext* context, IntList* itemIDs);
void armoryPopulateSell(ArmoryContext* context);
void armoryUpdateMenuText(COIMenu* menu, ArmoryItem* items, int numItems);
void armoryUpdateBoardText(COIBoard* board);
void armoryUpdateMoneyString(ArmoryContext* context);
void armoryDisableConfirmMenu(ArmoryContext* context);
void armoryEnableConfirmMenu(ArmoryContext* context);
void armoryBuyItem(COIBoard* board);
void armorySellItem(COIBoard* board);

// Private functions
// JNW: cleanup: remove private function declarations in header
int _textIDFromItemID(int item);
int _priceFromItemID(int item);
ArmoryContext* _armoryCreateContext(COIBoard* board, COIBoard* outsideBoard, COIWindow* window, Inventory* inventory, IntList* itemIDs);
char* _stringFromItemID(int item);

#endif
