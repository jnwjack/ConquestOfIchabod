#ifndef TOWN_H
#define TOWN_H

#include "../player.h"
#include "PauseOverlay.h"
#include "../TextBox.h"

// After how many ticks should we check for battle?
#define TOWN_BATTLE_TICKS 50
#define TOWN_MOVE_SPEED 4
#define TOWN_NUM_NPCS 3

// After this many ticks, chance for each NPC to move
#define TOWN_NPC_MOVEMENT_TICKS 40

#define TOWN_TENTACLE_MOVEMENT_TICKS 4

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
  bool willEnterBattle;
  COITextType* textType;
  PauseOverlay* pauseOverlay;
  TextBox* textBox;
  COIBoard* board;
  Actor* npcs[TOWN_NUM_NPCS];
  LinkedList* topTentacles;
  
  unsigned int _npcTicks;
} TownContext;

COIBoard* townCreateBoard(COIWindow* window, COIAssetLoader* loader, PlayerInfo* pInfo);

void townCheckForBattle(TownContext* context);
void townUpdateTerrain(TownContext* context, int collisionResult);
void townProcessDirectionalInput(TownContext* context, int direction);
void townProcessCollisionType(TownContext* context, int collision);
int townCheckForCollision(TownContext* context, Actor* actor);
void townTick(TownContext* context);
void townMoveNPCs(TownContext* context);
void townMovePlayer(TownContext* context);
bool townShouldCheckForCollision(TownContext* context);
void townTogglePauseOverlay(TownContext* context);
void townProcessSelectionInput(TownContext* context);
void townDestroyBoard(TownContext* context);

#endif
