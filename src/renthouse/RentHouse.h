#ifndef RENTHOUSE_H
#define RENTHOUSE_H

#include "../player.h"
#include "../TextBox.h"

#define RENT_HOUSE_HEAL 0
#define RENT_HOUSE_SLEEP 1
#define RENT_HOUSE_PAY 2
#define RENT_HOUSE_EXIT 3

#define RENT_HOUSE_PRICE 1200

#define RENT_HOUSE_REFRESH_MONEY 1
#define RENT_HOUSE_REFRESH_DAYS 2
#define RENT_HOUSE_REFRESH_PRICE 4

// JNW: Try to make this a "flat" structure, no unnecessary mallocs
typedef struct RHStatusWindow {
  COISprite* frame;
  COIString* currentGold;
  COIString* daysLeft;
  COIString* price;
  char _refreshFlags;
} RHStatusWindow;

typedef struct RentHouseContext {
  PlayerInfo* pInfo;
  COIMenu* baseMenu;
  COIMenu* confirmMenu;
  COIBoard* board;
  COIBoard* outsideBoard;
  COITextType* textType;
  TextBox* textBox;
  RHStatusWindow statusWindow;
} RentHouseContext;

COIBoard* RentHouseCreateBoard(PlayerInfo* pInfo, COIBoard* outsideBoard);
void RentHouseProcessDirectionalInput(RentHouseContext* context, int event);
bool RentHouseProcessSelectionInput(RentHouseContext* context);
void RentHouseDestroyBoard(RentHouseContext* context);
void RentHouseTick(RentHouseContext* context);

#endif
