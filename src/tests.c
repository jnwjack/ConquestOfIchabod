#include "tests.h"

Inventory* createTestInventory(ItemList* items) {
  Inventory* inventory = inventoryCreate(items);

  inventory->weapon = ItemListGetItem(items, ITEM_ID_RUSTY_SWORD);
  inventory->offHand = ItemListGetItem(items, ITEM_ID_CRACKED_SHIELD);
  inventory->head = ItemListGetItem(items, ITEM_ID_BRONZE_HELM);
  inventory->body = ItemListGetItem(items, ITEM_ID_UNARMORED_BODY);
  inventory->legs = ItemListGetItem(items, ITEM_ID_UNARMORED_LEGS);

  inventory->numBackpackItems = 4;
  inventory->numEquippedItems = 3;
  
  inventory->backpack[0] = ItemListGetItem(items, ITEM_ID_STRENGTH_POTION);
  inventory->backpack[1] = ItemListGetItem(items, ITEM_ID_SHABBY_BOW);
  inventory->backpack[2] = ItemListGetItem(items, ITEM_ID_RUSTY_SWORD);
  inventory->backpack[3] = ItemListGetItem(items, ITEM_ID_STRENGTH_POTION);

  inventory->money = 9999;

  return inventory;
}
