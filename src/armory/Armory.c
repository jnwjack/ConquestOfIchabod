#include "Armory.h"

void armorySetItem(ArmoryItem* item, int itemID, int textID, int price, int stock) {
  item->textID = textID;
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
  context->buyItems = malloc(sizeof(context->numBuyItems));

  
  // Hardcoded prices and stock values
  armorySetItem(&context->buyItems[0], ITEM_ID_RUSTY_SWORD, 3, 20, 1);
  armorySetItem(&context->buyItems[1], ITEM_ID_RUSTY_BATTLEAXE, 4, 28, 1);
  armorySetItem(&context->buyItems[2], ITEM_ID_SHABBY_BOW, 5, 20, 1);
  armorySetItem(&context->buyItems[3], ITEM_ID_CRACKED_SHIELD, 6, 10, 1);
  armorySetItem(&context->buyItems[4], ITEM_ID_STRENGTH_POTION, 7, 10, 5);
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


