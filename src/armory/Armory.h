#include "../engine/COIMenu.h"
#include "../engine/COIBoard.h"
#include "../engine/COIWindow.h"

#ifndef ARMORY_H
#define ARMORY_H

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
} ArmoryContext;

#endif
