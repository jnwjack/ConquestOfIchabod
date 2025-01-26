#ifndef ARMORY_H
#define ARMORY_H

#include "../player.h"
#include "../inputloops.h"

#define SELL_FACTOR 0.4

#define ARMORY_MAX_BUY_ITEMS 20
#define ARMORY_MAX_SELL_ITEMS 110

typedef struct ArmoryItem {
  int itemID;
  int slot;
  int stock;
  int price;
} ArmoryItem;

typedef struct ArmoryContext {
  COIBoard* outsideBoard; // Board to load when leaving shop
  COIBoard* board;
  COIMenu* menu;
  // may want to wrap submenu in a "buymenu" object or something
  COIMenu* buyMenu;
  COIMenu* sellMenu;
  COIMenu* currentMenu;
  COIMenu* confirmMenu;
  bool confirmActive;
  // List of items available for buying/selling in shop
  ArmoryItem buyItems[ARMORY_MAX_BUY_ITEMS];
  ArmoryItem sellItems[ARMORY_MAX_SELL_ITEMS];
  // ArmoryItem *buyItems;
  // ArmoryItem *sellItems;
  int numBuyItems;
  int numSellItems;
  Inventory* inventory; // Player inventory
  PlayerInfo* pInfo;
  COITextType* textType;
  COIString* mainStrings[3];
  COIString* confirmStrings[2];
  COIString* moneyString;
  bool isGag;
} ArmoryContext;

void armoryBuyItem(ArmoryContext* context);
void armorySellItem(ArmoryContext* context);
void armoryDisableConfirmMenu(ArmoryContext* context);
void armoryEnableConfirmMenu(ArmoryContext* context);
void armoryDestroy(ArmoryContext* context);
bool armorySelect(ArmoryContext* context);
void armoryBack(ArmoryContext* context);
void armoryProcessDirectionalInput(ArmoryContext* context, int direction);

COIBoard* armoryCreateBoardForPotionStore(COIBoard* outsideBoard, PlayerInfo* pInfo);
COIBoard* armoryCreateBoardForGeneralStore(COIBoard* outsideBoard, PlayerInfo* pInfo);
COIBoard* armoryCreateBoardForWeaponsStore(COIBoard* outsideBoard, PlayerInfo* pInfo);

#endif