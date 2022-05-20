#include "Armory.h"

void armorySetItem(ArmoryContext* context, int itemIndex, int textIndex, int bPrice, int sPrice, int stock) {
  if (context->numItems <= itemIndex) {
    printf("Error: Item index out of range\n");
    return;
  }
  context->buyItems[itemIndex].textIndex = textIndex;
  context->buyItems[itemIndex].buyPrice = bPrice;
  context->buyItems[itemIndex].sellPrice = sPrice;
  context->buyItems[itemIndex].stock = stock;
}

// Traverse array of items and get text indices
int* armoryGetTextIndices(ArmoryContext* context) {
  int* textIndices = malloc(sizeof(int) * context->numItems);
  
  int i;
  for (i = 0; i < context->numItems; i++) {
    textIndices[i] = context->buyItems[i].textIndex;
  }

  return textIndices;
}

void armorySetTextIndices(ArmoryContext* context, int* indices) {
  for (int i = 0; i < context->numItems; i++) {
    context->buyItems[i].textIndex = indices[i];
  }
}

void armoryBuyMenu(ArmoryContext* context, int selected) {
}

