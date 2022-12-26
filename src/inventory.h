#ifndef INVENTORY_H
#define INVENTORY_H

#include "items.h"

#define BACKPACK_SIZE 100

typedef struct Inventory {
  Item* head;
  Item* body;
  Item* legs;
  Item* weapon;
  Item* offHand;
  Item** backpack;
  int numBackpackItems;
} Inventory;

Inventory* inventoryCreate();

#endif
