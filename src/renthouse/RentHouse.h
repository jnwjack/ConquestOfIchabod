#ifndef RENTHOUSE_H
#define RENTHOUSE_H

#include "../engine/COIBoard.h"
#include "../engine/COISprite.h"
#include "../engine/COIWindow.h"
#include "../engine/COIMenu.h"
#include "../engine/COITextType.h"
#include "../player.h"
#include "../TextBox.h"

#define RENT_HOUSE_HEAL 0
#define RENT_HOUSE_SLEEP 1
#define RENT_HOUSE_PAY 2
#define RENT_HOUSE_EXIT 3

typedef struct RentHouseContext {
  PlayerInfo* pInfo;
  COIMenu* baseMenu;
  COIBoard* board;
  COIBoard* outsideBoard;
  COITextType* textType;
  TextBox* textBox;
} RentHouseContext;

COIBoard* RentHouseCreateBoard(PlayerInfo* pInfo, COIBoard* outsideBoard);
void RentHouseProcessDirectionalInput(RentHouseContext* context, int event);
bool RentHouseProcessSelectionInput(RentHouseContext* context);
void RentHouseDestroyBoard(RentHouseContext* context);
void RentHouseTick(RentHouseContext* context);

#endif
