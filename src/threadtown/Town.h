#ifndef TOWN_H
#define TOWN_H

#include "../engine/COIBoard.h"
#include "../engine/COIWindow.h"
#include "../player.h"

#define MOVING_NONE 0
#define MOVING_LEFT 1
#define MOVING_RIGHT 2
#define MOVING_UP 3
#define MOVING_DOWN 4

typedef struct TownContext {
  PlayerInfo* pInfo;
  COIWindow* window;
  int direction;
} TownContext;

COIBoard* townCreateBoard(COIWindow* window, COIAssetLoader* loader, PlayerInfo* pInfo);
void townDestroyBoard(COIBoard* board);

#endif
