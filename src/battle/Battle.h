#ifndef BATTLE_H
#define BATTLE_H

#include "../engine/COIBoard.h"
#include "../engine/COIWindow.h"
#include "../engine/COIMenu.h"
#include "../engine/COIString.h"
#include "../engine/COISprite.h"
#include "../actor.h"

typedef struct BattleContext {
  // Actions: Attack, Tech, Special, etc.
  COIMenu* actionMenu;
  COIString* actionStrings[5];

  COITextType* textType;

  Actor** enemies;
  int numEnemies;
} BattleContext;

COIBoard* battleCreateBoard(COIWindow* window, COIAssetLoader* loader, COIBoard* outsideBoard, int actorType);
void battleDestroyBoard(COIBoard* board);


#endif
