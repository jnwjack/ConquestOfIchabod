#ifndef RENTHOUSE_H
#define RENTHOUSE_H

#include "../engine/COIBoard.h"
#include "../engine/COISprite.h"
#include "../engine/COIWindow.h"
#include "../engine/COIMenu.h"
#include "../engine/COITextType.h"
#include "../player.h"

typedef struct RentHouseContext {
  PlayerInfo* pInfo;
  COIMenu* baseMenu;
  COIBoard* board;
  COITextType* textType;
} RentHouseContext;

COIBoard* RentHouseCreateBoard();

#endif
