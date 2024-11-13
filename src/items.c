
#include "items.h"
#include "engine/util.h"

#ifdef __WINDOWS__

/* This code is public domain -- Will Hartung 4/9/09 */
size_t getline(char **lineptr, size_t *n, FILE *stream) {
    char *bufptr = NULL;
    char *p = bufptr;
    size_t size;
    int c;

    if (lineptr == NULL) {
        return -1;
    }
    if (stream == NULL) {
        return -1;
    }
    if (n == NULL) {
        return -1;
    }
    bufptr = *lineptr;
    size = *n;

    c = fgetc(stream);
    if (c == EOF) {
        return -1;
    }
    if (bufptr == NULL) {
        bufptr = malloc(128);
        if (bufptr == NULL) {
            return -1;
        }
        size = 128;
    }
    p = bufptr;
    while(c != EOF) {
        if ((p - bufptr) > (size - 1)) {
            size = size + 128;
            bufptr = realloc(bufptr, size);
            if (bufptr == NULL) {
                return -1;
            }
        }
        *p++ = c;
        if (c == '\n') {
            break;
        }
        c = fgetc(stream);
    }

    *p++ = '\0';
    *lineptr = bufptr;
    *n = size;

    return p - bufptr - 1;
}
#endif

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

// int ItemAdjustStat(int baseStat, int itemStrength) {
//   float modifier = (float)(itemStrength / 10.0);
//   printf("MODIFIER: %f\n", MAX(1.0, modifier));
//   return baseStat * MAX(1.0, modifier);
// }

int ItemAdjustStat(int baseStat, int itemStrength) {
  return baseStat + itemStrength;
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
    return "STR Scroll";
  case ITEM_ID_DEF_SCROLL:
    return "DEF Scroll";
  case ITEM_ID_AGI_SCROLL:
    return "AGI Scroll";
  case ITEM_ID_BRONZE_HELM:
    return "Helm";
  case ITEM_ID_BRONZE_CHEST:
    // return "Chestplate";
    return "Chainmail";
  case ITEM_ID_BRONZE_LEGS:
    return "Greaves";
  case ITEM_ID_GEM_OF_PERMANENCE:
    return "Gem of Time";
  case ITEM_ID_TAGNESSE:
    return "Tagnesse";
  case ITEM_ID_KNIFE:
    return "Knife";
  case ITEM_ID_HEALING_POTION:
    return "HP Potion";
  case ITEM_ID_SP_POTION:
    return "SP Potion";
  case ITEM_ID_TP_POTION:
    return "TP Potion";
  case ITEM_ID_MOUNTAIN_JUICE:
    return "MNTN Juice";
  case ITEM_ID_LEATHER_ARMOR:
    return "Leather";
  case ITEM_ID_BOOTS:
    return "Boots";
  default:
    return "-";
  }
}

bool ItemCanUseInBattle(Item* item) {
  switch (item->id) {
  case ITEM_ID_STRENGTH_POTION:
  case ITEM_ID_DEF_SCROLL:
  case ITEM_ID_AGI_SCROLL:
  case ITEM_ID_HEALING_POTION:
  case ITEM_ID_TP_POTION:
  case ITEM_ID_SP_POTION:
  case ITEM_ID_MOUNTAIN_JUICE:
    return true;
  default:
    return false;
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

