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
  if (actorIsDead(actor)) {
    action.type = INACTIVE;
    return action;
  }
  
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
  _sortHelper(actions, 0, numActions - 1);
}

ActionSummary* battleBehaviorDoAction(BattleAction* action, char* playerName, COITextType* textType, COIBoard* board, COISprite* box) {
  Actor* a = action->actor;
  Actor* t = action->target;
  char* aName;
  char* tName;
  char atkString[MAX_STRING_SIZE];
  char dmgString[MAX_STRING_SIZE];  
  int damage = 0;
  ActionSummary* summary;
  if (a->actorType == ACTOR_PLAYER) {
    aName = playerName;
  } else {
    aName = actorGetNameFromType(a->actorType);
  }
  if (t->actorType == ACTOR_PLAYER) {
    tName = playerName;
  } else {
    tName = actorGetNameFromType(t->actorType);
  }


  switch (action->type) {
  case ATTACK:
    damage = MAX(1, a->atk - t->def);
    sprintf(atkString, "%s ATTACKS %s", aName, tName);
    sprintf(dmgString, "%i DAMAGE DEALT", damage);
    t->hp = MAX(0, t->hp - damage);
    if (actorIsDead(t)) {
      t->sprite->_visible = false;
      char deathString[MAX_STRING_SIZE];
      sprintf(deathString, "%s DIES", tName);
      summary = ActionSummaryCreate(board, box, textType,
				    atkString,
				    dmgString,
				    deathString,
				    NULL);
    } else {
      summary = ActionSummaryCreate(board, box, textType, atkString, dmgString, NULL);
    }
    //printf("%s ATTACKS %s FOR %i DAMAGE\n", aName, tName, damage);
    break;
  default:
    printf("Invalid action type.\n");
    summary = ActionSummaryCreate(board, box, textType, "Invalid action type.");
  }
  
  return summary;
}

ActionSummary* ActionSummaryCreate(COIBoard* board, COISprite* box, COITextType* textType, char* string, ...) {
  ActionSummary* summary = malloc(sizeof(ActionSummary));
  summary->strings = LinkedListCreate();
    
  va_list list;
  char* currentString = string;
  va_start(list, string);
  
  // Count how many strings we have
  /*int count = 0;
  while (currentString){
    count++;
    currentString = va_arg(list, char*);
  }
  va_end(list);
  summary->numStrings = count;
  summary->strings = malloc(sizeof(COIString*) * count);*/
  summary->ticksPerString = 50; // Can have an arg for this later
  summary->ticks = 0;
  summary->finished = false;

  // Create a COIString for each string arg
  //currentString = string;
  va_start(list, string);
  //int i = 0;
  summary->numStrings = 0;
  while (currentString){
    COIString* coiString = COIStringCreate(currentString, 0, 0, textType);
    //summary->strings[i] 
    COIStringConfineToSprite(coiString, box);
    // Add COIString to board
    COIBoardAddString(board, coiString);
    COIStringSetVisible(coiString, false);
    LinkedListAdd(summary->strings, (void*)coiString);
    currentString = va_arg(list, char*);
    //i++;
    summary->numStrings++;
  }
  // Set first string to visible
  if (summary->numStrings > 0) {
    LinkedListResetCursor(summary->strings);
    summary->currentStringObj = (COIString*)LinkedListNext(summary->strings);
    COIStringSetVisible(summary->currentStringObj, true);
    summary->currentString = 0;
    //COIStringSetVisible(summary->strings[0], true);
  }
  va_end(list);

  
  
  return summary;
}

void ActionSummaryAdvance(ActionSummary* summary, bool skipToNextString) {
  // Can't skip past last string
  if (summary->currentString + 1 >= summary->numStrings) {
    skipToNextString = false;
  }
  
  summary->ticks++;
  if (summary->ticks >= summary->ticksPerString || skipToNextString) {
    summary->ticks = 0;
    COIStringSetVisible(summary->currentStringObj, false);
    summary->currentString++;
    if (summary->currentString >= summary->numStrings) {
      summary->finished = true;
    } else {
      summary->currentStringObj = (COIString*)LinkedListNext(summary->strings);
      COIStringSetVisible(summary->currentStringObj, true);
    }
  }
}

void ActionSummaryAddString(ActionSummary* summary, char* newString) {
  LinkedListAdd(summary->strings, (void*)newString);
  summary->numStrings++;
}

void ActionSummaryPosition(ActionSummary* summary, int x, int y) {
  /*
  for (int i = 0; i < summary->numStrings; i++) {
    COIStringSetPos(summary->strings[i], x, y);
    }*/
  // This function would break things if we mess with the linked list cursor
}

void ActionSummaryDestroy(ActionSummary* summary, COIBoard* board) {
  LinkedListResetCursor(summary->strings);
  COIString* string = LinkedListNext(summary->strings);
  while (string) {
    COIBoardRemoveString(board, string);
    COIStringDestroy(string);
    string = LinkedListNext(summary->strings);
  }
  LinkedListDestroy(summary->strings);
  //free(summary->strings);
  free(summary);
}


