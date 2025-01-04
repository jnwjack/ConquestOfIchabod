#ifndef TOWN_H
#define TOWN_H

#include "../player.h"
#include "PauseOverlay.h"
#include "../TextBox.h"
#include "../terrain.h"

// After how many ticks should we check for battle?
#define TOWN_BATTLE_TICKS 50
#define TOWN_MOVE_SPEED 4
#define TOWN_NUM_NPC_CITIZENS 15

// After this many ticks, chance for each NPC to move
#define TOWN_NPC_MOVEMENT_TICKS 40

#define TOWN_TENTACLE_MOVEMENT_TICKS 20

#define TOWN_MUSIC_X_TRIGGER 4224

typedef struct TownContext {
  PlayerInfo* pInfo;
  int direction;
  Terrain terrain;
  bool willEnterBattle;
  COITextType* textType;
  PauseOverlay* pauseOverlay;
  TextBox* textBox;
  COIBoard* board;

  // Dynamic sprites
  Actor* npcs[TOWN_NUM_NPC_CITIZENS];
  Actor* swordChest;
  LinkedList* allActors;


  int talkingActorType; // Who are we talking to?
  LinkedList* topTentacles;
  COIMenu* confirmMenu;
  
  bool _creepy;
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
void townApplyTimeChanges(TownContext* context);
bool townShopIsClosed();
void townCheckForTiredMessage(TownContext* context);

#endif
