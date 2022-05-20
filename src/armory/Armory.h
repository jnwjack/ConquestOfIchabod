#include "../engine/COIMenu.h"
#include "../engine/COIBoard.h"
#include "../engine/COIWindow.h"

#ifndef ARMORY_H
#define ARMORY_H

typedef struct ArmoryItem {
  int id;
  int textIndex;
  int stock;
  int buyPrice;
  int sellPrice;
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
  COIMenu* currentMenu;
  // List of items available for buying/selling in shop
  ArmoryItem* buyItems;
  ArmoryItem* sellItems;
  int numItems;
} ArmoryContext;


void armorySetItem(ArmoryContext* context, int itemIndex, int textIndex, int bPrice, int sPrice, int stock);
int* armoryGetTextIndices(ArmoryContext* context);
void armorySetTextIndices(ArmoryContext* context, int* indices);

// Menu Behavior
void armoryBuyMenu(ArmoryContext* context, int selected);

#endif
