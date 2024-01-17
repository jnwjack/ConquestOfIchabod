#ifndef INVENTORY_H
#define INVENTORY_H

#include "items.h"
#include <stdbool.h>

#define BACKPACK_SIZE 100
#define MAX_MONEY 9999

typedef struct Inventory {
  ItemList* items;
  
  Item* head;
  Item* body;
  Item* legs;
  Item* weapon;
  Item* offHand;
  Item** backpack;
  int numBackpackItems;
  int numEquippedItems;

  int money;
} Inventory;

Inventory* inventoryCreate(ItemList* items);
void inventoryDestroy(Inventory* inventory);
bool inventoryAddItem(Inventory* inventory, int itemID);
bool inventoryRemoveBackpackItem(Inventory* inventory, int index);
bool inventoryRemoveBackpackItemFirstInstance(Inventory* inventory, Item* item);
bool inventoryRemoveEquippedItem(Inventory* inventory, int slot);
Item** inventoryGetEquippedItems(Inventory* inventory);

#endif
