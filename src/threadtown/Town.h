#ifndef TOWN_H
#define TOWN_H

#include "../engine/COIBoard.h"
#include "../engine/COIWindow.h"
#include "../player.h"
#include "../engine/util.h"
#include "../engine/COIExtraCollision.h"

// After how many ticks should we check for battle?
#define TOWN_BATTLE_TICKS 50
#define TOWN_MOVE_SPEED 4

// What kind of terrain are we on? Used when determining if we should have an encounter
typedef enum TownTerrain {
  TT_THICK_GRASS,
  TT_SAFE // No chance of battle
} TownTerrain;

typedef struct TownContext {
  PlayerInfo* pInfo;
  COIWindow* window;
  int direction;
  TownTerrain terrain;
  TownTerrain nextGridTerrain;
  int terrainTicks;
  COIBoard* board;
} TownContext;

COIBoard* townCreateBoard(COIWindow* window, COIAssetLoader* loader, PlayerInfo* pInfo);

bool townCheckForBattle(TownContext* context);
void townUpdateTerrain(TownContext* context, int collisionResult);
void townProcessMovementInput(TownContext* context, int direction);
void townProcessCollisionType(TownContext* context, int collision);
int townCheckForCollision(TownContext* context);
void townMovePlayer(TownContext* context);
bool townShouldCheckForCollision(TownContext* context);


void townDestroyBoard(COIBoard* board);

#endif
