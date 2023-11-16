#ifndef TOWN_H
#define TOWN_H

#include "../engine/COIBoard.h"
#include "../engine/COIWindow.h"
#include "../player.h"

typedef struct TownContext {
  PlayerInfo* pInfo;
  COIWindow* window;
  int direction;
} TownContext;

COIBoard* townCreateBoard(COIWindow* window, COIAssetLoader* loader, PlayerInfo* pInfo);
void townDestroyBoard(COIBoard* board);

#endif
