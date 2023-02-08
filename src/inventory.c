#include "inventory.h"

Inventory* inventoryCreate(ItemList* items) {
  Inventory* inventory = malloc(sizeof(Inventory));
  inventory->backpack = malloc(sizeof(Item*) * BACKPACK_SIZE);
  inventory->items = items;
  return inventory;
}

void inventoryDestroy(Inventory* inventory) {
  free(inventory->backpack);
  free(inventory);
}

bool inventoryAddItem(Inventory* inventory, int itemID) {
  if (inventory->numBackpackItems == BACKPACK_SIZE) {
    return false;
  }
  inventory->backpack[inventory->numBackpackItems] = ItemListGetItem(inventory->items, itemID);
  inventory->numBackpackItems++;
  return true;
}

