#include "inventory.h"

Inventory* inventoryCreate(ItemList* items) {
  Inventory* inventory = malloc(sizeof(Inventory));
  inventory->backpack = malloc(sizeof(Item*) * BACKPACK_SIZE);
  inventory->items = items;
  inventory->numEquippedItems = 0;
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

bool inventoryRemoveBackpackItem(Inventory* inventory, int index) {
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

bool inventoryRemoveEquippedItem(Inventory* inventory, int slot) {
  switch (slot) {
  case ITEM_SLOT_WEAPON:
    if (inventory->weapon->id != ITEM_ID_UNARMED) {
      inventory->weapon = ItemListGetItem(inventory->items, ITEM_ID_UNARMED);
      inventory->numEquippedItems--;
      break;
    }
    return false;
  case ITEM_SLOT_OFFHAND:
    if (inventory->offHand->id != ITEM_ID_UNARMED_OFF) {
      inventory->offHand = ItemListGetItem(inventory->items, ITEM_ID_UNARMED_OFF);
      inventory->numEquippedItems--;
      break;
    }
    return false;
  case ITEM_SLOT_HEAD:
    if (inventory->head->id != ITEM_ID_UNARMORED_HEAD) {
      inventory->head = ItemListGetItem(inventory->items, ITEM_ID_UNARMORED_HEAD);
      inventory->numEquippedItems--;
      break;
    }
    return false;
  case ITEM_SLOT_BODY:
    if (inventory->body->id != ITEM_ID_UNARMORED_BODY) {
      inventory->body = ItemListGetItem(inventory->items, ITEM_ID_UNARMORED_BODY);
      inventory->numEquippedItems--;
      break;
    }
    return false;
  case ITEM_SLOT_LEGS:
    if (inventory->legs->id != ITEM_ID_UNARMORED_LEGS) {
      inventory->legs = ItemListGetItem(inventory->items, ITEM_ID_UNARMORED_LEGS);
      inventory->numEquippedItems--;
      break;
    }
    return false;
  }
  return true;
}

Item** inventoryGetEquippedItems(Inventory* inventory) {
  Item** equippedItems = malloc(sizeof(Item*) * inventory->numEquippedItems);

  int count = 0;
  if (inventory->head->id != ITEM_ID_UNARMORED_HEAD) {
    equippedItems[count++] = inventory->head;
  }
  if (inventory->body->id != ITEM_ID_UNARMORED_BODY) {
    equippedItems[count++] = inventory->body;
  }
  if (inventory->legs->id != ITEM_ID_UNARMORED_LEGS) {
    equippedItems[count++] = inventory->legs;
  }
  if (inventory->weapon->id != ITEM_ID_UNARMED) {
    equippedItems[count++] = inventory->weapon;
  }
  if (inventory->offHand->id != ITEM_ID_UNARMED_OFF) {
    equippedItems[count++] = inventory->offHand;
  }

  return equippedItems;
}

