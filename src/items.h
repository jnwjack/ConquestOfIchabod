#include <string.h>
#include <stdlib.h>

#ifndef ITEMS_H
#define ITEMS_H

#define ITEM_SLOT_WEAPON 0;
#define ITEM_SLOT_HEAD 1;
#define ITEM_SLOT_BODY 2;
#define ITEM_SLOT_LEGS 3;
#define ITEM_SLOT_NA 4;
#define ITEM_SLOT_OFFHAND 5;

#define ITEMS_FILE "src/items.dat"

// Item IDs -------------------
#define ITEM_ID_RUSTY_SWORD 0
#define ITEM_ID_RUSTY_BATTLEAXE 1
#define ITEM_ID_SHABBY_BOW 2
#define ITEM_ID_CRACKED_SHIELD 3
#define ITEM_ID_STRENGTH_POTION 4
// ----------------------------

typedef enum {
  WEAPON,
  ARMOR,
  CONSUMABLE
} ItemType;

typedef struct Item {
  ItemType type;
  int strength;
  int slot;
} Item;

typedef struct ItemList {
  Item* items;
  int numItems;
} ItemList;

// Load global item list, we'll index to this from other places (armory, etc.)
ItemList* loadItems();
void ItemListDestroy(ItemList* list);

#endif
