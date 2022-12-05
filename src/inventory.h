#include "items.h"

#define BACKPACK_SIZE = 100

typedef struct Inventory {
  Item* head;
  Item* body;
  Item* legs;
  Item* weapon;
  Item* offHand;
  Item** backpack;
} Inventory;

Inventory* InventoryCreate() {
  Inventory* inventory = malloc(sizeof(Inventory));
  inventory->backpack = malloc(sizeof(Item*) * BACKPACK_SIZE);
  return inventory;
}
