#include "Armory.h"

void armorySetItem(ArmoryItem* item, int itemID, int price, int stock) {
  item->textID = armoryTextIDFromItemID(itemID);
  item->itemID = itemID;
  item->stock = stock;
  item->price = price;
}

// Traverse array of items and get text indices
int* armoryGetTextIndices(ArmoryContext* context) {
  int* textIndices = malloc(sizeof(int) * context->numBuyItems);
  
  int i;
  for (i = 0; i < context->numBuyItems; i++) {
    textIndices[i] = context->buyItems[i].textID;
  }

  return textIndices;
}

void armorySetTextIndices(ArmoryContext* context, int* indices) {
  for (int i = 0; i < context->numBuyItems; i++) {
    context->buyItems[i].textID = indices[i];
  }
}

// Initialize the "buy" menu items
void armoryPopulateBuy(ArmoryContext* context) {
  context->numBuyItems = 5;
  context->buyItems = malloc(context->numBuyItems * sizeof(ArmoryItem));

  
  // Hardcoded prices and stock values
  armorySetItem(&context->buyItems[0], ITEM_ID_RUSTY_SWORD, 20, 1);
  armorySetItem(&context->buyItems[1], ITEM_ID_RUSTY_BATTLEAXE, 28, 1);
  armorySetItem(&context->buyItems[2], ITEM_ID_SHABBY_BOW, 20, 1);
  armorySetItem(&context->buyItems[3], ITEM_ID_CRACKED_SHIELD, 10, 1);
  armorySetItem(&context->buyItems[4], ITEM_ID_STRENGTH_POTION, 10, 5);
}

void armoryBuyMenu(ArmoryContext* context, int selected) {
}

void armoryDestroy(ArmoryContext* context) {
  if (context->buyItems != NULL) {
    free(context->buyItems);
  }
  if (context->sellItems != NULL) {
    free(context->sellItems);
  }
  free(context);
}

// Given an item ID, return the proper text as defined in armory/text.dat
// We may want to re-implement this in a more global way (e.g. text.dat for all items)
int armoryTextIDFromItemID(int item) {
  switch (item) {
  case ITEM_ID_RUSTY_SWORD:
    return 3;
    break;
  case ITEM_ID_RUSTY_BATTLEAXE:
    return 4;
    break;
  case ITEM_ID_SHABBY_BOW:
    return 5;
    break;
  case ITEM_ID_CRACKED_SHIELD:
    return 6;
    break;
  case ITEM_ID_STRENGTH_POTION:
    return 7;
    break;
  default:
    printf("Error: No valid text ID\n");
    return -1;
  }
}
