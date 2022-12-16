#include "../engine/COIMenu.h"
#include "../engine/COIBoard.h"
#include "../engine/COIWindow.h"
#include "../inventory.h"

#ifndef ARMORY_H
#define ARMORY_H

// Can later replace this with charisma or something
#define SELL_FACTOR 0.75

typedef struct ArmoryItem {
  int itemID;
  int textID;
  int stock;
  int price;
} ArmoryItem;

// Set of indices to texts in file that indicate the armory's stock.
// there should be a armory object that holds current stock, prices, mappings to text object, etc.
typedef struct ArmoryContext {
  int pointerLocation;
  COIBoard* board;
  COIWindow* window;
  COIMenu* menu;
  // may want to wrap submenu in a "buymenu" object or something
  COIMenu* buyMenu;
  COIMenu* sellMenu;
  COIMenu* currentMenu;
  // List of items available for buying/selling in shop
  ArmoryItem* buyItems;
  ArmoryItem* sellItems;
  int numBuyItems;
  int numSellItems;
} ArmoryContext;

void armoryDestroy(ArmoryContext* context);

void armorySetItem(ArmoryItem* item, int itemID, int stock);
int* armoryGetTextIndices(ArmoryContext* context);
void armorySetTextIndices(ArmoryContext* context, int* indices);
void armoryPopulateBuy(ArmoryContext* context);

// Menu Behavior
void armoryBuyMenu(ArmoryContext* context, int selected);


// Private functions
int _textIDFromItemID(int item);
int _priceFromItemID(int item);

#endif
