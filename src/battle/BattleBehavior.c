#include "BattleBehavior.h"
#include "../special.h"
#include "../items.h"
#include <assert.h>

// Action with actor having higher AGI is smaller
int _compareActions(BattleAction a, BattleAction b) {
  int aAgi = actorModifiedAgi(a.actor);
  int bAgi = actorModifiedAgi(b.actor);

  if (aAgi > bAgi) {
    return -1;
 } else if (aAgi < bAgi) {
    return 1;
  } else {
    return 0;
  }
}

// QuickSort
void _sortHelper(BattleAction* actions, int lo, int hi) {
  // BattleAction* actions = *actionsPtr;

  int size = (hi+1) - lo;
  if (size < 2) {
    return;
  }

  int pivot = lo + (size / 2);
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
  _sortHelper(actions, lo, store - 1);
  _sortHelper(actions, store + 1, hi);
}

void _initializeAction(BattleAction* action) {
  action->attackModifier = 1.0;
  action->spCostModifier = 1.0;
  action->damageAttacker = false;
}

// Breaks if you call this and there are no alive actors provided
Actor* _pickRandomAliveActor(Actor** actors, int numActors) {
  Actor* aliveActors[numActors];
  int numAliveActors = 0;
  for (int i = 0; i < numActors; i++) {
    if (!actorIsDead(actors[i])) {
      aliveActors[numAliveActors++] = actors[i];
    }
  }

  int selectedActor = generateRandomCharInRange(0, numAliveActors - 1);
  return aliveActors[selectedActor];
}

ActionType battleBehaviorPickActionType(int actorType) {
  // In future, can have different behavior for different actors.
  switch (actorType) {
  case ACTOR_TENTACLE:
  case ACTOR_WIRE_MOTHER:
  case ACTOR_VOLCANETTE:
  case ACTOR_BOOWOW:
    return (generateRandomBoolWeighted(0.25) ? SPECIAL : ATTACK);
  default:
    return ATTACK;
  }
}

// Handle behavior that's specific to each special
void battleBehaviorMakeSpecial(BattleAction* action, int specialID, int targetIndex, Actor** targetActors, int numTargetActors, Actor* actor) {
  action->actor = actor;
  action->type = SPECIAL;
  action->target = targetActors[targetIndex];
  action->index = specialID;
  action->successfulFlee = false;
  action->numOtherTargets = 0;
  if (specialID == SPECIAL_ID_AVALANCHE) {
    // Special case: also do damage to adjacent targets
    int targetAbove = targetIndex - 1;
    int targetBelow = targetIndex + 1;
    if (targetAbove >= 0 && !actorIsDead(targetActors[targetAbove])) {
      action->otherTargets[action->numOtherTargets] = targetActors[targetAbove];
      // action->otherTargets[action->numOtherTargets] = NULL;
      action->numOtherTargets++;
    } 
    if (targetBelow < numTargetActors && !actorIsDead(targetActors[targetBelow])) {
      // action->otherTargets[action->numOtherTargets] = NULL;
      action->otherTargets[action->numOtherTargets] = targetActors[targetBelow];
      action->numOtherTargets++;
    }
  }
}

Actor* battleBehaviorPickTarget(int actorType, BattleAction* action, Actor** enemies, int numEnemies, Actor** allies, int numAllies) {
  switch (action->type) {
  case ATTACK:
    return _pickRandomAliveActor(enemies, numEnemies);
  case SPECIAL:
    if (specialTargetsEnemies(action->index)) {
      return _pickRandomAliveActor(enemies, numEnemies);
    } else {
      return _pickRandomAliveActor(allies, numAllies);
    }
  default:
    return NULL;
  }
}

int battleBehaviorPickIndex(ActionType action, Actor* actor) {
  switch (action) {
  case SPECIAL:
    {
      int index = generateRandomCharInRange(0, actor->specials.length - 1);
      return actor->specials.values[index];
    }
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
  action.index = battleBehaviorPickIndex(action.type, actor);
  action.target = battleBehaviorPickTarget(actor->actorType, &action, actorEnemies, numEnemies, actorAllies, numAllies);
  action.numOtherTargets = 0;
  if (actorIsDead(actor)) {
    action.type = INACTIVE;
    return action;
  }

  

  return action;
}

void battleBehaviorSwapActions(BattleAction* a, BattleAction* b) {
  BattleAction temp = *a;
  a->actor = b->actor;
  a->target = b->target;
  a->type = b->type;
  a->index = b->index;
  a->attackModifier = b->attackModifier;
  a->spCostModifier = b->spCostModifier;
  a->damageAttacker = b->damageAttacker;
  a->successfulFlee = b->successfulFlee;
  a->numOtherTargets = b->numOtherTargets;
  for (int i = 0; i < a->numOtherTargets; i++) {
    a->otherTargets[i] = b->otherTargets[i];
  }

  b->actor = temp.actor;
  b->target = temp.target;
  b->type = temp.type;
  b->index = temp.index;
  b->attackModifier = temp.attackModifier;
  b->spCostModifier = temp.spCostModifier;
  b->damageAttacker = temp.damageAttacker;
  b->successfulFlee = temp.successfulFlee;
  b->numOtherTargets = temp.numOtherTargets;
  for (int i = 0; i < b->numOtherTargets; i++) {
    b->otherTargets[i] = temp.otherTargets[i];
  }
}



void battleBehaviorSortActions(BattleAction* actionsPtr, int numActions) {
  // BattleAction* actions = *actionsPtr;
  _sortHelper(actionsPtr, 0, numActions - 1);
}

ActionSummary* battleBehaviorDoAction(BattleAction* action, COITextType* textType, COIBoard* board, COISprite* box, PlayerInfo* pInfo, LinkedList* modifiers) {
  Actor* a = action->actor;
  Actor* t = action->target;
  char* aName;
  char* tName;
  char atkString[MAX_STRING_SIZE];
  char dmgString[MAX_STRING_SIZE];  
  int damage = 0;
  ActionSummary* summary;
  _initializeAction(action);

  // Apply actor TECH
  for (int i = 0; i < a->techList->count; i++) {
    Tech* tech = a->techList->techs[i];
    if (tech->active) {
      a->tp -= tech->cost;
      switch (tech->id) {
      case TECH_ID_FOCUS:
        action->spCostModifier *= tech->strength;
        break;
      case TECH_ID_RAGE:
        action->attackModifier *= tech->strength;
        break;
      }
    }
  }

  // Apply target TECH
  for (int i = 0; i < t->techList->count; i++) {
    Tech* tech = t->techList->techs[i];
    if (tech->active) {
      switch (tech->id) {
      case TECH_ID_COUNTER:
        action->damageAttacker = true;
        break;
      case TECH_ID_BRACE:
        action->attackModifier *= tech->strength;
        break;
      case TECH_ID_RAGE:
        action->attackModifier *= tech->strength;
        break;
      }
    }
  }

  int aAtk, aDef, tAtk, tDef;
  if (a->actorType == ACTOR_PLAYER) {
    aName = pInfo->name;
    aAtk = playerAdjustedATK(pInfo);
    aDef = playerAdjustedDEF(pInfo);
  } else {
    aName = actorGetNameFromType(a->actorType);
    aAtk = actorModifiedAtk(a);
    aDef = actorModifiedDef(a);
  }
  if (t->actorType == ACTOR_PLAYER) {
    tName = pInfo->name;
    tAtk = playerAdjustedATK(pInfo);
    tDef = playerAdjustedDEF(pInfo);
  } else {
    tName = actorGetNameFromType(t->actorType);
    tAtk = actorModifiedAtk(t);
    tDef = actorModifiedDef(t);
  }

  // JNW: cleanup. Pull each action type out into its own function
  char temp[MAX_STRING_SIZE];
  SpecialType spType; 
  bool actionFails = false;
  switch (action->type) {
  case ATTACK:
    damage = MAX(1, aAtk - tDef) * action->attackModifier;
    sprintf(atkString, "%s ATTACKS %s", aName, tName);
    summary = ActionSummaryCreate(board, box, textType, atkString, NULL);

    // Check for modifiers that affect an incoming attack
    LinkedListResetCursor(modifiers);
    ActorBattleModifier* currentModifier = (ActorBattleModifier*)LinkedListNext(modifiers);
    while (currentModifier) {
      if (currentModifier->actor == t && currentModifier->type == MT_PARRYING) {
        int parryDamage = damage * 2;
        a->hp = MAX(0, a->hp - parryDamage);
        snprintf(temp, MAX_STRING_SIZE, "%s PARRIES!", tName);
        ActionSummaryAddString(summary, temp, board, box, textType);
        snprintf(temp, MAX_STRING_SIZE, "%i DAMAGE DEALT TO %s", parryDamage, aName);
        ActionSummaryAddString(summary, temp, board, box, textType);
        actionFails = true;
      }
      currentModifier = LinkedListNext(modifiers);
    }

    if (actionFails) {
      break;
    }

    // Save attack value and defense value in BattleAction on creation
    t->hp = MAX(0, t->hp - damage);
    sprintf(dmgString, "%i DAMAGE DEALT", damage);
    ActionSummaryAddString(summary, dmgString, board, box, textType);
    if (action->damageAttacker) {
      a->hp = MAX(0, a->hp - damage / 2);
      snprintf(temp, MAX_STRING_SIZE, "%s COUNTERS!", tName);
      ActionSummaryAddString(summary, temp, board, box, textType);
      snprintf(temp, MAX_STRING_SIZE, "%i DAMAGE DEALT TO %s", damage / 2, aName);
      ActionSummaryAddString(summary, temp, board, box, textType);
    }
    break;
  case SPECIAL:
    spType = specialType(action->index);
    a->sp -= (specialCost(action->index) * action->spCostModifier);
    if (spType == SPECIAL_DAMAGING) {
      damage = specialStrength(action->index);
      t->hp = MAX(0, t->hp - damage);
      
      snprintf(temp, MAX_STRING_SIZE, "%s %s %s ON %s",
	       aName, specialVerb(action->index), specialName(action->index), tName);
      summary = ActionSummaryCreate(board, box, textType, temp, NULL);
      snprintf(temp, MAX_STRING_SIZE, "%i DAMAGE DEALT", damage);
      ActionSummaryAddString(summary, temp, board, box, textType);

      for (int i = 0; i < action->numOtherTargets; i++) {
        Actor* otherT = action->otherTargets[i];
        damage = specialSecondaryStrength(action->index);
        otherT->hp = MAX(0, otherT->hp - damage);

        snprintf(temp, MAX_STRING_SIZE, "%s ALSO TAKES %i DAMAGE",
	        actorGetNameFromType(otherT->actorType), damage);
        ActionSummaryAddString(summary, temp, board, box, textType);
      }
    } else if (spType == SPECIAL_HEALING) {
      int amountHealed = MIN(specialStrength(action->index), t->hpMax - t->hp);
      LinkedListResetCursor(modifiers);
      ActorBattleModifier* currentModifier = (ActorBattleModifier*)LinkedListNext(modifiers);
      while (currentModifier) {
        if (currentModifier->actor == t && currentModifier->type == MT_CURSED) {
          t->hp = MAX(0, a->hp - amountHealed);
          snprintf(temp, MAX_STRING_SIZE, "%s IS CURSED!", tName);
          ActionSummaryAddString(summary, temp, board, box, textType);
          snprintf(temp, MAX_STRING_SIZE, "%i DAMAGE DEALT TO %s", amountHealed, aName);
          ActionSummaryAddString(summary, temp, board, box, textType);
          actionFails = true;
        }
        currentModifier = LinkedListNext(modifiers);
      }
      if (actionFails) {
        break;
      }

      t->hp = MIN(t->hp + specialStrength(action->index), t->hpMax);
      snprintf(temp, MAX_STRING_SIZE, "%s %s %s ON %s",
	       aName, specialVerb(action->index), specialName(action->index), tName);
      summary = ActionSummaryCreate(board, box, textType, temp, NULL);
      snprintf(temp, MAX_STRING_SIZE, "%i HP RESTORED", amountHealed);
      ActionSummaryAddString(summary, temp, board, box, textType);
    } else if (action->index == SPECIAL_ID_PARRY) {
      snprintf(temp, MAX_STRING_SIZE, "%s %s %s",
	       aName, specialVerb(action->index), specialName(action->index));
      summary = ActionSummaryCreate(board, box, textType, temp, NULL);
      // Only handling parry right now
      ActorBattleModifier* modifier = malloc(sizeof(ActorBattleModifier));
      modifier->actor = a;
      modifier->turnsLeft = 0;
      modifier->type = MT_PARRYING;
      LinkedListAdd(modifiers, (void*)modifier);
    } else if (action->index == SPECIAL_ID_CURSE) {
      snprintf(temp, MAX_STRING_SIZE, "%s %s %s ON %s",
	       aName, specialVerb(action->index), specialName(action->index), tName);
      summary = ActionSummaryCreate(board, box, textType, temp, NULL);
      // Only handling curse right now
      ActorBattleModifier* modifier = malloc(sizeof(ActorBattleModifier));
      modifier->actor = t;
      modifier->turnsLeft = 3;
      modifier->type = MT_CURSED;
      LinkedListAdd(modifiers, (void*)modifier);
    } else if (action->index == SPECIAL_ID_DRAIN_SPIRIT) {
      int spLost = specialStrength(action->index);
      snprintf(temp, MAX_STRING_SIZE, "%s %s %s ON %s",
	       aName, specialVerb(action->index), specialName(action->index), tName);
      summary = ActionSummaryCreate(board, box, textType, temp, NULL);
      t->sp = MAX(0, t->sp - spLost);
      snprintf(temp, MAX_STRING_SIZE, "SP REDUCED BY %i", spLost);
      ActionSummaryAddString(summary, temp, board, box, textType);
    } else if (action->index == SPECIAL_ID_REINFORCE) {
      snprintf(temp, MAX_STRING_SIZE, "%s %s %s ON %s",
	       aName, specialVerb(action->index), specialName(action->index), tName);
      summary = ActionSummaryCreate(board, box, textType, temp, NULL);
      TimeStateCopyGlobalTime(&t->def.end);
      TimeStateAddVal(&t->def.end, 1);
      if (!TimeStateInFuture(&t->def.end) || t->def.factor < 1) {
        t->def.factor = 1.5;
      } else {
        t->def.factor *= 1.5;
      }
    } else {
      summary = ActionSummaryCreate(board, box, textType, "Invalid action type", NULL);
    }
    break;
  case ITEM:
    actorUseConsumable(t, &pInfo->inventory->items->items[action->index]);
    snprintf(temp, MAX_STRING_SIZE, "%s USES %s ON %s", aName, ItemListStringFromItemID(action->index), tName);
    summary = ActionSummaryCreate(board, box, textType, temp, NULL);
    break;
  case FLEE:
    snprintf(temp, MAX_STRING_SIZE, "%s TRIES TO FLEE", aName);
    summary = ActionSummaryCreate(board, box, textType, temp, NULL);
    if (actorModifiedAgi(action->target) > actorModifiedAgi(action->actor)) {
      snprintf(temp, MAX_STRING_SIZE, "BLOCKED BY %s!", tName);
      ActionSummaryAddString(summary, temp, board, box, textType);
      action->successfulFlee = false;
    } else {
      action->successfulFlee = true;
      ActionSummaryAddString(summary, "YOU ESCAPE!", board, box, textType);
    }
    break;
  default:
    printf("Invalid action type.\n");
    summary = ActionSummaryCreate(board, box, textType, "Invalid action type", NULL);
  }


  // Check for death
  if (actorIsDead(a)) {
    a->sprite->_autoHandle = false;
    a->sprite->_visible = false;
    snprintf(temp, MAX_STRING_SIZE, "%s DIES", aName);
    ActionSummaryAddString(summary, temp, board, box, textType);
  }
  if (actorIsDead(t)) {
    t->sprite->_autoHandle = false;
    t->sprite->_visible = false;
    snprintf(temp, MAX_STRING_SIZE, "%s DIES", tName);
    ActionSummaryAddString(summary, temp, board, box, textType);
  }
  for (int i = 0; i < action->numOtherTargets; i++) {
    Actor* otherT = action->otherTargets[i];
    if (actorIsDead(otherT)) {
      otherT->sprite->_autoHandle = false;
      otherT->sprite->_visible = false;
      snprintf(temp, MAX_STRING_SIZE, "%s DIES", actorGetNameFromType(otherT->actorType));
      ActionSummaryAddString(summary, temp, board, box, textType);
    }
  }


  return summary;
}

void battleBehaviorUpdateModifiersTimeLeft(LinkedList* modifiers) {
  // Check for modifiers that affect an incoming attack
  LinkedListResetCursor(modifiers);
  ActorBattleModifier* currentModifier = (ActorBattleModifier*)LinkedListNext(modifiers);
  while (currentModifier) {
    if (currentModifier->turnsLeft == 0) {
      LinkedListRemove(modifiers, (void*)currentModifier);
    } else {
      currentModifier->turnsLeft--;
    }
    currentModifier = LinkedListNext(modifiers);
  }
}

void battleBehaviorsDestroyModifiers(LinkedList* modifiers) {
  LinkedListResetCursor(modifiers);
  ActorBattleModifier* currentModifier = (ActorBattleModifier*)LinkedListNext(modifiers);
  while (currentModifier) {
    free(currentModifier);
    currentModifier = LinkedListNext(modifiers);
  }
  LinkedListDestroy(modifiers);
}

ActionSummary* ActionSummaryCreate(COIBoard* board, COISprite* box, COITextType* textType, char* string, ...) {
  ActionSummary* summary = malloc(sizeof(ActionSummary));
  summary->strings = LinkedListCreate();
    
  va_list list;
  char* currentString = string;
  va_start(list, string);
  
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
    summary->currentStringObj = (COIString*)LinkedListNext(summary->strings);
    if (summary->currentStringObj == NULL) {
      summary->finished = true;
    } else {
      COIStringSetVisible(summary->currentStringObj, true);
    }
    /*
    if (summary->currentString >= summary->numStrings) {
      summary->finished = true;
      } else {
      summary->currentStringObj = (COIString*)LinkedListNext(summary->strings);
      if (summary->currentStringObj == NULL) {
	printf("bad: %i\n", summary->currentString);
      }
      COIStringSetVisible(summary->currentStringObj, true);
    }
    */
  }
}

void ActionSummaryAddString(ActionSummary* summary, char* newString, COIBoard* board, COISprite* sprite, COITextType* textType) {
  COIString* coiString = COIStringCreate(newString, 0, 0, textType);
  //summary->strings[i] 
  COIStringConfineToSprite(coiString, sprite);
  // Add COIString to board
  COIBoardAddString(board, coiString);
  COIStringSetVisible(coiString, false);
  LinkedListAdd(summary->strings, (void*)coiString);
  LinkedListResetCursor(summary->strings);
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


