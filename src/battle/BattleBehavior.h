#ifndef BATTLEBEHAVIOR_H
#define BATTLEBEHAVIOR_H

#include "../actor.h"

typedef enum {
  ATTACK,
  SPECIAL,
  ITEM,
  FLEE,
  INACTIVE
} ActionType;

typedef struct BattleAction {
  Actor* actor; // Who is acting?
  Actor* target; // Targeted actor of attack/special/item
  ActionType type;
  int index; // Index of used special/item
} BattleAction;

// AI action selection
ActionType battleBehaviorPickActionType(int actorType);
Actor* battleBehaviorPickTarget(int actorType, ActionType action, Actor** enemies, int numEnemies, Actor** allies, int numAllies);
int battleBehaviorPickIndex(ActionType action, TechList* techList);
BattleAction battleBehaviorGenerateAction(Actor* actor, Actor** actorEnemies, int numEnemies, Actor** actorAllies, int numAllies);

// Sorting
void battleBehaviorSwapActions(BattleAction* a, BattleAction* b);
void battleBehaviorSortActions(BattleAction* actions, int numActions);

void battleBehaviorDoAction(BattleAction* action, char* playerName);


#endif
