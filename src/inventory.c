#include "inventory.h"
#include <stdio.h>

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
  
  Item* item = ItemListGetItem(inventory->items, itemID);
  if (ItemIsUnarmedItem(item)) {
    return false;
  }
  
  inventory->backpack[inventory->numBackpackItems] = item;
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

// Like the above function, but we don't care about the specific index.
// Just search the list for the index of the
// first instance of the provided item, then remove it.
bool inventoryRemoveBackpackItemFirstInstance(Inventory* inventory, Item* item) {
  for (int i = 0; i < inventory->numBackpackItems; i++) {
    if (inventory->backpack[i]->id == item->id) {
      return inventoryRemoveBackpackItem(inventory, i);
    }
  }

  return false;
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

// Equip item and return the item it replaced
Item* inventoryEquipItem(Inventory* inventory, Item* item) {
  Item* oldItem;
  switch (item->slot) {
  case ITEM_SLOT_WEAPON:
    oldItem = inventory->weapon;
    inventory->weapon = item;
    break;
  case ITEM_SLOT_OFFHAND:
    oldItem = inventory->offHand;
    inventory->offHand = item;
    break;
  case ITEM_SLOT_HEAD:
    oldItem = inventory->head;
    inventory->head = item;
    break;
  case ITEM_SLOT_BODY:
    oldItem = inventory->body;
    inventory->body = item;
    break;
  case ITEM_SLOT_LEGS:
    oldItem = inventory->legs;
    inventory->legs = item;
    break;
  }

  // If slot was previously empty and now isn't, we now have 1 more equipped item.
  if (ItemIsUnarmedItem(oldItem)) {
    inventory->numEquippedItems++;
  }

  // Of course, we may have equipped one of these 'empty slot' items, which we mean we have 1 less equipped item
  if (ItemIsUnarmedItem(item)) {
    inventory->numEquippedItems--;
  }

  return oldItem;
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

int inventoryDEFItemTotalStrength(Inventory* inventory) {
  int total = (inventory->head->strength +
	       inventory->body->strength +
	       inventory->legs->strength);
  if (inventory->offHand->type == ARMOR) {
    total += inventory->offHand->strength;
  }

  return total;
}

