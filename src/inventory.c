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

bool inventoryRemoveItem(Inventory* inventory, int index) {
  if (inventory->numBackpackItems == 0) {
    return false;
  }
  // Rebuild backpack
  Item** newBackpack = malloc(sizeof(Item*) * BACKPACK_SIZE);
  int indexInNewBackpack = 0;
  for (int i = 0; i < inventory->numBackpackItems; i++) {
    if (i != index) {
      newBackpack[indexInNewBackpack] = inventory->backpack[i];
      indexInNewBackpack++;
    }
  }

  free(inventory->backpack);
  inventory->backpack = newBackpack;
  inventory->numBackpackItems--;
  return true;
}

