#include "BattleBehavior.h"

// Action with actor having higher AGI is larger
int _compareActions(BattleAction a, BattleAction b) {
  if (a.actor->agi > b.actor->agi) {
    return 1;
  } else if (a.actor->agi < b.actor->agi) {
    return -1;
  } else {
    return 0;
  }
}

// QuickSort
void _sortHelper(BattleAction* actions, int lo, int hi) {
  int size = hi - lo;
  if (size < 2) {
    return;
  }

  int pivot = size / 2;
  // Swap pivot with rightmost element
  battleBehaviorSwapActions(&actions[pivot], &actions[hi]);

  int store = lo; // Values at positions less than this are smaller than the pivot
  BattleAction pivotAction = actions[hi];
  for (int i = lo; i < hi; i++) {
    BattleAction currentAction = actions[i];
    if (_compareActions(currentAction, pivotAction) < 1) {
      // The current action should be left of the pivot
      battleBehaviorSwapActions(&actions[i], &actions[store]);
      store++;
    }
  }

  // Swap store with rightmost, pivot is now in its final place
  battleBehaviorSwapActions(&actions[store], &actions[hi]);

  // Recurse
  _sortHelper(actions, lo, pivot - 1);
  _sortHelper(actions, pivot + 1, hi);
}

ActionType battleBehaviorPickActionType(int actorType) {
  // In future, can have different behavior for different actors.
  return ATTACK;
}

Actor* battleBehaviorPickTarget(int actorType, ActionType action, Actor** enemies, int numEnemies, Actor** allies, int numAllies) {
  switch (action) {
  case ATTACK:
    return enemies[0]; // For now, just pick first enemy
  default:
    return NULL;
  }
}

int battleBehaviorPickIndex(ActionType action, TechList* techList) {
  switch (action) {
  case SPECIAL:
  case ITEM:
    return 0; // For now, first in list
  default:
    return -1;
  }
}

BattleAction battleBehaviorGenerateAction(Actor* actor, Actor** actorEnemies, int numEnemies, Actor** actorAllies, int numAllies) {
  BattleAction action;
  action.actor = actor;
  action.type = battleBehaviorPickActionType(actor->actorType);
  action.target = battleBehaviorPickTarget(actor->actorType, action.type, actorEnemies, numEnemies, actorAllies, numAllies);
  action.index = battleBehaviorPickIndex(actor->actorType, actor->techList);

  return action;
}

void battleBehaviorSwapActions(BattleAction* a, BattleAction* b) {
  BattleAction temp = *a;
  
  a->actor = b->actor;
  a->target = b->target;
  a->type = b->type;
  a->index = b->index;

  b->actor = temp.actor;
  b->target = temp.target;
  b->type = temp.type;
  b->index = temp.index;
}



void battleBehaviorSortActions(BattleAction* actions, int numActions) {
  for (int i = 0; i < numActions; i++) {
    printf("AGI IN SORT:%i\n", actions[i].actor->agi);
  }
  
  _sortHelper(actions, 0, numActions - 1);
}


