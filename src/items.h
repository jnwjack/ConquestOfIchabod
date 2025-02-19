#ifndef ITEMS_H
#define ITEMS_H

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define ITEM_SLOT_WEAPON 0
#define ITEM_SLOT_HEAD 1
#define ITEM_SLOT_BODY 2
#define ITEM_SLOT_LEGS 3
#define ITEM_SLOT_NA 4
#define ITEM_SLOT_OFFHAND 5

#define ITEMS_FILE "src/items.dat"

// Item IDs -------------------
#define ITEM_ID_RUSTY_SWORD 0
#define ITEM_ID_RUSTY_BATTLEAXE 1
#define ITEM_ID_SHABBY_BOW 2
#define ITEM_ID_CRACKED_SHIELD 3
#define ITEM_ID_BRONZE_HELM 4
#define ITEM_ID_BRONZE_CHEST 5
#define ITEM_ID_BRONZE_LEGS 6
#define ITEM_ID_STRENGTH_POTION 7
#define ITEM_ID_UNARMED 8
#define ITEM_ID_UNARMORED_HEAD 9
#define ITEM_ID_UNARMORED_BODY 10
#define ITEM_ID_UNARMORED_LEGS 11
#define ITEM_ID_UNARMED_OFF 12
#define ITEM_ID_GEM_OF_PERMANENCE 13
#define ITEM_ID_TAGNESSE 14
#define ITEM_ID_KNIFE 15
#define ITEM_ID_HEALING_POTION 16
#define ITEM_ID_SP_POTION 17
#define ITEM_ID_TP_POTION 18
#define ITEM_ID_MOUNTAIN_JUICE 19
#define ITEM_ID_DEF_SCROLL 20
#define ITEM_ID_AGI_SCROLL 21
#define ITEM_ID_LEATHER_ARMOR 22
#define ITEM_ID_BOOTS 23

// Gag items for armory
#define ITEM_ID_SLEEP_IN 24
#define ITEM_ID_READ_BOOK 25
#define ITEM_ID_MAKE_FRIEND 26
#define ITEM_ID_TAKE_WALK 27
#define ITEM_ID_EXERCISE 28
#define ITEM_ID_RUN_ERRANDS 29
#define ITEM_ID_COOK 30
#define ITEM_ID_GARDEN 31
#define ITEM_ID_DATE 32
#define ITEM_ID_NEW_HOBBY 33
// ----------------------------

typedef enum {
  WEAPON,
  ARMOR,
  CONSUMABLE
} ItemType;

typedef struct Item {
  ItemType type;
  int id;
  int strength;
  int slot;
} Item;

typedef struct ItemList {
  Item* items;
  int numItems;
} ItemList;

// Load global item list, we'll index to this from other places (armory, etc.)
ItemList* loadItems();
Item* ItemListGetItem(ItemList* list, int index);
void ItemListDestroy(ItemList* list);
char* ItemListStringFromItemID(int item);

// General function for affecting stat values via item
int ItemAdjustStat(int baseStat, int itemStrength);

bool ItemIsUnarmedItem(Item* item);
bool ItemCanUseInBattle(Item* item);


#endif
