#ifndef BATTLE_H
#define BATTLE_H

#include "../engine/COIBoard.h"
#include "../engine/COIWindow.h"
#include "../engine/COIMenu.h"
#include "../engine/COIString.h"
#include "../engine/COISprite.h"
#include "../actor.h"

#define BATTLE_ATTACK 0
#define BATTLE_TECH 1
#define BATTLE_SPECIAL 2
#define BATTLE_ITEM 3
#define BATTLE_FLEE 4

#define BATTLE_POINTER_PADDING_X 10

typedef struct BattleContext {
  // Actions: Attack, Tech, Special, etc.
  COIMenu* actionMenu;
  COIString* actionStrings[5];
  bool actionMenuFocused;

  COITextType* textType;

  Actor** enemies;
  COIString** enemyNames;
  int numEnemies;

  // Pointer for enemies and allies
  COISprite* pointer;
  // Is the pointer over the enemies or the allies?
  bool pointingAtEnemies;
  // Who are we looking at?
  int targetedActorIndex;
} BattleContext;

COIBoard* battleCreateBoard(COIWindow* window, COIAssetLoader* loader, COIBoard* outsideBoard, int actorType);
void battleDestroyBoard(COIBoard* board);

void battleHandleActionSelection(BattleContext* context);
void battleMovePointer(BattleContext* context, int offset);


#endif
