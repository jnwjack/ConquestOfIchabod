#include "inventory.h"

Inventory* inventoryCreate() {
  Inventory* inventory = malloc(sizeof(Inventory));
  inventory->backpack = malloc(sizeof(Item*) * BACKPACK_SIZE);
  return inventory;
}
