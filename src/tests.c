#include "tests.h"

Inventory* createTestInventory(ItemList* items) {
  Inventory* inventory = malloc(sizeof(Inventory));

  inventory->weapon = ItemListGetItem(items, ITEM_ID_RUSTY_SWORD);
  inventory->offHand = ItemListGetItem(items, ITEM_ID_CRACKED_SHIELD);

  return inventory;
}
