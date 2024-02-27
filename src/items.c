
#include "items.h"
#include "engine/util.h"

// Load item data in via dat file
ItemList* loadItems() {
  ItemList* list = malloc(sizeof(ItemList));
  list->numItems = countLines(ITEMS_FILE);

  list->items = malloc(sizeof(Item) * list->numItems);

  FILE* fp = fopen(ITEMS_FILE, "r");
  if (fp == NULL) {
    free(list->items);
    free(list);
    return NULL;
  }
  char* line = NULL;
  size_t len = 0;
  char type;
  int i = 0;
  while (getline(&line, &len, fp) != -1) {
    Item* currentItem = &list->items[i];
    line[strcspn(line, "\n")] = '\0';

    type = strtok(line, " ")[0];
    switch (type) {
    case 'W':
      currentItem->type = WEAPON;
      break;
    case 'A':
      currentItem->type = ARMOR;
      break;
    case 'C':
      currentItem->type = CONSUMABLE;
      break;
    default:
      printf("Error: Invalid item type at %i", i);
    }

    currentItem->strength = atoi(strtok(NULL, " "));
    currentItem->slot = atoi(strtok(NULL, " "));
    currentItem->id = i;

    i++;
  }

  fclose(fp);
  if (line) {
    free(line);
  }

  return list;
}

Item* ItemListGetItem(ItemList* list, int index) {
  if (index >= list->numItems) {
    printf("Index %i out of ItemList range\n", index);
    exit(1);
  }

  return &list->items[index];
}

int ItemAdjustStat(int baseStat, int itemStrength) {
  return baseStat * (float)(itemStrength / 10.0);
}

char* ItemListStringFromItemID(int item) {
  switch (item) {
  case ITEM_ID_RUSTY_SWORD:
    return "Sword";
  case ITEM_ID_RUSTY_BATTLEAXE:
    return "Battleaxe";
  case ITEM_ID_SHABBY_BOW:
    return "Bow";
  case ITEM_ID_CRACKED_SHIELD:
    return "Shield";
  case ITEM_ID_STRENGTH_POTION:
    return "STR Potion";
  case ITEM_ID_BRONZE_HELM:
    return "Helm";
  case ITEM_ID_BRONZE_CHEST:
    return "Chestplate";
  case ITEM_ID_BRONZE_LEGS:
    return "Greaves";
  default:
    return "-";
  }
}

// True if this item represents an unfilled item slot
bool ItemIsUnarmedItem(Item* item) {
  return (item->id >= ITEM_ID_UNARMED && item->id <= ITEM_ID_UNARMED_OFF);
}

void ItemListDestroy(ItemList* list) {
  if (list == NULL) {
    return;
  }

  free(list->items);
  free(list);
}

