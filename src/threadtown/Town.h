#ifndef TOWN_H
#define TOWN_H

#include "../player.h"
#include "PauseOverlay.h"
#include "../TextBox.h"
#include "../terrain.h"

// After how many ticks should we check for battle?
#define TOWN_BATTLE_TICKS 50
#define TOWN_MOVE_SPEED 4
#define TOWN_NUM_NPCS 5

// After this many ticks, chance for each NPC to move
#define TOWN_NPC_MOVEMENT_TICKS 40

#define TOWN_TENTACLE_MOVEMENT_TICKS 20

typedef struct TownContext {
  PlayerInfo* pInfo;
  COIWindow* window;
  int direction;
  Terrain terrain;
  bool willEnterBattle;
  int battleActorType;
  COITextType* textType;
  PauseOverlay* pauseOverlay;
  TextBox* textBox;
  COIBoard* board;
  Actor* npcs[TOWN_NUM_NPCS];
  int talkingActorType; // Who are we talking to?
  LinkedList* topTentacles;
  COIMenu* confirmMenu;
  
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
