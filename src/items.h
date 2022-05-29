#include <string.h>
#include <stdlib.h>

#define ITEM_SLOT_WEAPON 0;
#define ITEM_SLOT_HEAD 1;
#define ITEM_SLOT_BODY 2;
#define ITEM_SLOT_LEGS 3;
#define ITEM_SLOT_NA 4;

#define ITEMS_FILE "src/items.dat"

typedef enum {
  WEAPON,
  ARMOR,
  CONSUMABLE
} ItemType;

typedef struct Item {
  ItemType type;
  int strength;
  int price;
  int slot;
} Item;

typedef struct ItemList {
  Item* items;
  int numItems;
} ItemList;

// Load global item list, we'll index to this from other places (armory, etc.)
ItemList* loadItems();
void ItemListDestroy(ItemList* list);
