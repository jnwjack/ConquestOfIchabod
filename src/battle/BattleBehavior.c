#include "BattleBehavior.h"
#include "../special.h"
#include "../items.h"
#include <assert.h>

bool battleBehaviorCheckForModifiers(Actor* actor, ModifierType type, LinkedList* modifiers) {
  LinkedListResetCursor(modifiers);
  ActorBattleModifier* currentModifier = (ActorBattleModifier*)LinkedListNext(modifiers);
  while (currentModifier) {
    if (currentModifier->actor == actor && currentModifier->type == type) {
      return true;
    }
    currentModifier = LinkedListNext(modifiers);
  }

  return false;
}

static bool _critical() {
  return generateRandomBoolWeighted(0.1);
}

static int _randomDamage(int damage) {
  return MAX(1, generateRandomCharWithCenter(damage, MAX(1, damage / 10)));
}

static void _buffStat(Stat* stat) {
  TimeStateCopyGlobalTime(&stat->end);
  TimeStateAddVal(&stat->end, 1);
  if (!TimeStateInFuture(&stat->end) || stat->factor < 1) {
    stat->factor = 1.5;
  } else {
    stat->factor *= 1.5;
  }
  printf("FACTOR: %f\n", stat->factor);
}

// Action with actor having higher AGI is smaller
static int _compareActions(BattleAction a, BattleAction b) {
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
static void _sortHelper(BattleAction* actions, int lo, int hi) {
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

static void _initializeAction(BattleAction* action) {
  action->attackModifier = 1.0;
  action->spCostModifier = 1.0;
  action->damageAttacker = false;
}

// Breaks if you call this and there are no alive actors provided
static Actor* _pickRandomAliveActor(Actor** actors, int numActors) {
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

static void _damagingSpecial(BattleAction* action, ActionSummary* summary, COIBoard* board, COISprite* box, COITextType* textType) {
  Actor* t = action->target;
  int damage;
  char temp[MAX_STRING_SIZE];
  if (action->index == SPECIAL_ID_BACKSTAB) {
    int base = actorModifiedAgi(action->actor) - actorModifiedAgi(action->target);
    damage = _randomDamage(specialStrength(base));
  } else {
    damage = _randomDamage(specialStrength(action->index));
  }

  if (_critical()) {
    damage *= 2;
    // snprintf(temp, MAX_STRING_SIZE, "%s %s %s ON %s",
    //   aName, specialVerb(action->index), specialName(action->index), tName);
    // summary = ActionSummaryCreate(board, box, textType, temp, NULL);
    ActionSummaryAddString(summary, "CRITICAL HIT!", board, box, textType);
    snprintf(temp, MAX_STRING_SIZE, "%i DAMAGE DEALT", damage);
    ActionSummaryAddString(summary, temp, board, box, textType);
  } else {
    // snprintf(temp, MAX_STRING_SIZE, "%s %s %s ON %s",
    //   aName, specialVerb(action->index), specialName(action->index), tName);
    // summary = ActionSummaryCreate(board, box, textType, temp, NULL);
    snprintf(temp, MAX_STRING_SIZE, "%i DAMAGE DEALT", damage);
    ActionSummaryAddString(summary, temp, board, box, textType);
  }

  t->hp = MAX(0, t->hp - damage);

  for (int i = 0; i < action->numOtherTargets; i++) {
    Actor* otherT = action->otherTargets[i];
    damage = _randomDamage(specialSecondaryStrength(action->index));
    otherT->hp = MAX(0, otherT->hp - damage);

    snprintf(temp, MAX_STRING_SIZE, "%s ALSO TAKES %i DAMAGE",
      actorGetNameFromType(otherT->actorType), damage);
    ActionSummaryAddString(summary, temp, board, box, textType);

    if (actorIsDead(otherT)) {
      action->otherTargetsKilled[action->numOtherTargetsKilled] = otherT;
      action->numOtherTargetsKilled++;
    }
  }
}

static void _debuffSpecial(BattleAction* action, ActionSummary* summary, COIBoard* board, COISprite* box, COITextType* textType, LinkedList* modifiers, Actor* target) {
  char temp[MAX_STRING_SIZE];
  switch (action->index) {
  case SPECIAL_ID_CURSE: 
  {
    ActorBattleModifier* modifier = malloc(sizeof(ActorBattleModifier));
    modifier->actor = target;
    modifier->turnsLeft = 3;
    modifier->type = MT_CURSED;
    LinkedListAdd(modifiers, (void*)modifier);
    break;
  }
  case SPECIAL_ID_DRAIN_SPIRIT:
  {
    int spLost = specialStrength(action->index);
    target->sp = MAX(0, target->sp - spLost);
    snprintf(temp, MAX_STRING_SIZE, "SP REDUCED BY %i", spLost);
    ActionSummaryAddString(summary, temp, board, box, textType);
    break;
  }
  case SPECIAL_ID_NEUTRALIZE:
    target->def.factor = 1.0;
    target->agi.factor = 1.0;
    target->atk.factor = 1.0;
    ActionSummaryAddString(summary, "STATS RETURNED TO NORMAL", board, box, textType);
    break;
  case SPECIAL_ID_SILENCE:
  {
    ActorBattleModifier* modifier = malloc(sizeof(ActorBattleModifier));
    modifier->actor = target;
    modifier->turnsLeft = 5;
    modifier->type = MT_SILENCED;
    LinkedListAdd(modifiers, (void*)modifier);
    ActionSummaryAddString(summary, "SPECIAL ABILITIES CAN NO LONGER BE USED", board, box, textType);
    break;
  }
  default:
    printf("Error: Tried to process a debuff special we don't support.\n");
  }
}

ActionType battleBehaviorPickActionType(int actorType) {
  // In future, can have different behavior for different actors.
  switch (actorType) {
  case ACTOR_TENTACLE:
    return (generateRandomBoolWeighted(0.5) ? SPECIAL : ATTACK);
  case ACTOR_WIRE_MOTHER:
    return (generateRandomBoolWeighted(0.85) ? SPECIAL : ATTACK);
  case ACTOR_VOLCANETTE:
    return (generateRandomBoolWeighted(0.75) ? SPECIAL : ATTACK);
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
  action->numOtherTargetsKilled = 0;
  if (specialID == SPECIAL_ID_AVALANCHE || specialID == SPECIAL_ID_HOWL) {
    // Working with different enemy ordering
    for (int i = 0; i < numTargetActors; i++) {
      if (actorIsDead(targetActors[i])) {
        continue;
      }
      switch (i) {
      case 0:
        if (targetIndex == 1 || targetIndex == 2 || targetIndex == 3) {
          action->otherTargets[action->numOtherTargets] = targetActors[i];
          action->numOtherTargets++;
        } 
        break;
      case 1:
        if (targetIndex == 0 || targetIndex == 2 || targetIndex == 4) {
          action->otherTargets[action->numOtherTargets] = targetActors[i];
          action->numOtherTargets++;
        } 
        break;
      case 2:
        if (targetIndex != 2) {
          action->otherTargets[action->numOtherTargets] = targetActors[i];
          action->numOtherTargets++;
        } 
        break;
      case 3:
        if (targetIndex == 0 || targetIndex == 2) {
          action->otherTargets[action->numOtherTargets] = targetActors[i];
          action->numOtherTargets++;
        } 
        break;
      case 4:
        if (targetIndex == 1 || targetIndex == 2) {
          action->otherTargets[action->numOtherTargets] = targetActors[i];
          action->numOtherTargets++;
        } 
        break;
      default:
        printf("Error in handling ajacent targets\n.");
      }
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
    } else if (specialTargetsSelf(action->index)) {
      return action->actor;
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

void battleBehaviorGenerateAction(BattleAction* action, Actor* actor, Actor** actorEnemies, int numEnemies, Actor** actorAllies, int numAllies) {
  action->actor = actor;
  action->type = battleBehaviorPickActionType(actor->actorType);
  action->index = battleBehaviorPickIndex(action->type, actor);
  action->target = battleBehaviorPickTarget(actor->actorType, action, actorEnemies, numEnemies, actorAllies, numAllies);
  action->numOtherTargets = 0;
  action->numOtherTargetsKilled = 0;
  if (action->type == SPECIAL) {
    int targetIndex = 0; // This will break if we ever have more than one character in parties.
    // Will stay at 0 if we're targeting the player's party.
    for (int i = 0; i < numAllies; i++) {
      if (action->target == actorAllies[i]) {
        targetIndex = i;
        break;
      }
    }
    Actor** actors = specialTargetsEnemies(action->index) ? actorEnemies : actorAllies;
    int numActors = specialTargetsEnemies(action->index) ? numEnemies : numAllies;
    battleBehaviorMakeSpecial(action, action->index, targetIndex, actors, numActors, actor);
  }
  if (actorIsDead(actor)) {
    action->type = INACTIVE;
  }
}

void battleBehaviorSwapActions(BattleAction* a, BattleAction* b) {
  BattleAction temp = *a;
  for (int i = 0; i < a->numOtherTargets; i++) {
    temp.otherTargets[i] = a->otherTargets[i];
  }
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

  a->successfulFlee = false;
  b->successfulFlee = false;
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
  int damageBase, damage = 0;
  ActionSummary* summary;
  _initializeAction(action);
  double hitRate = BB_DEFAULT_HIT_RATE;
  bool damSpecialHitsTwice = false;

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
      case TECH_ID_EAGLEEYE:
        hitRate = 1.0;
        break;
      case TECH_ID_QUICKSTRIKE:
        damSpecialHitsTwice = true;
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
      case TECH_ID_FADE:
        hitRate = BB_FADE_HIT_RATE;
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

  printf("TARGET DEF BASE: %i\n", t->def.base);

  // JNW: cleanup. Pull each action type out into its own function
  char temp[MAX_STRING_SIZE];
  SpecialType spType; 
  bool actionFails = false;
  switch (action->type) {
  case ATTACK:
    damageBase = MAX(1, aAtk - tDef) * action->attackModifier;
    // Trying new damage calculation
    if (aAtk >= tDef) {
      damageBase = aAtk * 2 - tDef;
    } else {
      damageBase = (aAtk * aAtk) / tDef;
    }
    damage = _randomDamage(damageBase);
    bool successfulHit = generateRandomBoolWeighted(hitRate);
    if (successfulHit && _critical()) {
      damage *= 2;
      sprintf(atkString, "%s ATTACKS %s", aName, tName);
      summary = ActionSummaryCreate(board, box, textType, atkString, NULL);
      ActionSummaryAddString(summary, "CRITICAL HIT!", board, box, textType);
    } else {
      sprintf(atkString, "%s ATTACKS %s", aName, tName);
      summary = ActionSummaryCreate(board, box, textType, atkString, NULL);
    }
    if (successfulHit) {
      if (battleBehaviorCheckForModifiers(t, MT_PARRYING, modifiers)) {
        int parryDamage = damage * 2;
        a->hp = MAX(0, a->hp - parryDamage);
        snprintf(temp, MAX_STRING_SIZE, "%s PARRIES!", tName);
        ActionSummaryAddString(summary, temp, board, box, textType);
        snprintf(temp, MAX_STRING_SIZE, "%i DAMAGE DEALT TO %s", parryDamage, aName);
        ActionSummaryAddString(summary, temp, board, box, textType);

        // Action fails
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
    } else {
      ActionSummaryAddString(summary, "THE ATTACK MISSES!", board, box, textType);
    }
    break;
  case SPECIAL:
    if (battleBehaviorCheckForModifiers(a, MT_SILENCED, modifiers)) {
      summary = ActionSummaryCreate(board, box, textType, "SILENCED!", NULL);
      break;
    }
    spType = specialType(action->index);
    a->sp -= (specialCost(action->index) * action->spCostModifier);
    if (spType == SPECIAL_DAMAGING) {
      snprintf(temp, MAX_STRING_SIZE, "%s %s %s ON %s",
        aName, specialVerb(action->index), specialName(action->index), tName);
      summary = ActionSummaryCreate(board, box, textType, temp, NULL);
      if (!generateRandomBoolWeighted(hitRate)) {
        ActionSummaryAddString(summary, "THE ATTACK MISSES!", board, box, textType);
        break;
      }
      if (damSpecialHitsTwice) {
        _damagingSpecial(action, summary, board, box, textType);
        ActionSummaryAddString(summary, "THE ATTACK REPEATS!", board, box, textType);
      }
      _damagingSpecial(action, summary, board, box, textType);
    } else if (spType == SPECIAL_HEALING) {
      int amountHealed = MIN(specialStrength(action->index), t->hpMax - t->hp);
      if (battleBehaviorCheckForModifiers(t, MT_CURSED, modifiers)) {
        t->hp = MAX(0, a->hp - amountHealed);
        snprintf(temp, MAX_STRING_SIZE, "%s IS CURSED!", tName);
        summary = ActionSummaryCreate(board, box, textType, temp, NULL);
        snprintf(temp, MAX_STRING_SIZE, "%i DAMAGE DEALT TO %s", amountHealed, aName);
        ActionSummaryAddString(summary, temp, board, box, textType);
        
        // Action fails
        break;
      }
      t->hp = MIN(t->hp + specialStrength(action->index), t->hpMax);
      snprintf(temp, MAX_STRING_SIZE, "%s %s %s ON %s",
	       aName, specialVerb(action->index), specialName(action->index), tName);
      summary = ActionSummaryCreate(board, box, textType, temp, NULL);
      snprintf(temp, MAX_STRING_SIZE, "%i HP RESTORED", amountHealed);
      ActionSummaryAddString(summary, temp, board, box, textType);
    } else if (spType == SPECIAL_GAG) {
      snprintf(temp, MAX_STRING_SIZE, "%s %s %s", aName, specialVerb(action->index), specialName(action->index));
      summary = ActionSummaryCreate(board, box, textType, temp, "NOTHING HAPPENS!", NULL);
    } else if (spType == SPECIAL_DEBUFF) {
      snprintf(temp, MAX_STRING_SIZE, "%s %s %s ON %s",
	       aName, specialVerb(action->index), specialName(action->index), tName);
      summary = ActionSummaryCreate(board, box, textType, temp, NULL);
      if (generateRandomBoolWeighted(hitRate)) {
        _debuffSpecial(action, summary, board, box, textType, modifiers, t);
      } else {
        ActionSummaryAddString(summary, "THE ATTACK MISSES!", board, box, textType);
      }
      break;
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
    } else if (action->index == SPECIAL_ID_REINFORCE) {
      snprintf(temp, MAX_STRING_SIZE, "%s %s %s ON %s",
	       aName, specialVerb(action->index), specialName(action->index), tName);
      summary = ActionSummaryCreate(board, box, textType, temp, NULL);
      _buffStat(&t->def);
    } else if (action->index == SPECIAL_ID_HOWL || action->index == SPECIAL_ID_HASTE) {
      snprintf(temp, MAX_STRING_SIZE, "%s %s %s",
	       aName, specialVerb(action->index), specialName(action->index));
      summary = ActionSummaryCreate(board, box, textType, temp, NULL);
      _buffStat(&t->agi);
      snprintf(temp, MAX_STRING_SIZE, "%s BECOMES FASTER", aName);
      ActionSummaryAddString(summary, temp, board, box, textType);
      for (int i = 0; i < action->numOtherTargets; i++) {
        if (actorIsDead(action->otherTargets[i])) {
          continue;
        }
        _buffStat(&action->otherTargets[i]->agi);
        snprintf(temp, MAX_STRING_SIZE, "%s ALSO BECOMES FASTER", actorGetNameFromType(action->otherTargets[i]->actorType));
        ActionSummaryAddString(summary, temp, board, box, textType);
      }
    } else if (action->index == SPECIAL_ID_TIME_SKIP) {
      snprintf(temp, MAX_STRING_SIZE, "%s %s %s",
	       aName, specialVerb(action->index), specialName(action->index));
      summary = ActionSummaryCreate(board, box, textType, temp, NULL);
      ActionSummaryAddString(summary, "YOU ESCAPE!", board, box, textType);
      TimeStateIncrement(6);
      action->successfulFlee = true;
    } else {
      summary = ActionSummaryCreate(board, box, textType, "Invalid action type", NULL);
    }
    break;
  case ITEM: 
  {
    Item* item = &pInfo->inventory->items->items[action->index];
    snprintf(temp, MAX_STRING_SIZE, "%s USES %s ON %s", aName, ItemListStringFromItemID(action->index), tName);
    summary = ActionSummaryCreate(board, box, textType, temp, NULL);
    inventoryRemoveBackpackItemFirstInstance(pInfo->inventory,
        ItemListGetItem(pInfo->inventory->items, action->index));
    if (item->id == ITEM_ID_HEALING_POTION && battleBehaviorCheckForModifiers(t, MT_CURSED, modifiers)) {
      t->hp = MAX(0, t->hp - item->strength);
      snprintf(temp, MAX_STRING_SIZE, "%s IS CURSED!", tName);
      ActionSummaryAddString(summary, temp, board, box, textType);
      snprintf(temp, MAX_STRING_SIZE, "%i DAMAGE DEALT TO %s", item->strength, aName);
      ActionSummaryAddString(summary, temp, board, box, textType);
    } else {
      actorUseConsumable(t, &pInfo->inventory->items->items[action->index]);
    }
    break;
  }
  case FLEE:
    snprintf(temp, MAX_STRING_SIZE, "%s TRIES TO FLEE", aName);
    summary = ActionSummaryCreate(board, box, textType, temp, NULL);
    // Higher chance to succeed if we're faster than the target.
    if (actorModifiedAgi(action->target) <= actorModifiedAgi(action->actor)) {
      action->successfulFlee = generateRandomBoolWeighted(0.85);
    } else {
      action->successfulFlee = generateRandomBoolWeighted(0.5);
    }
    if (action->successfulFlee) {
      ActionSummaryAddString(summary, "YOU ESCAPE!", board, box, textType);
    } else {
      snprintf(temp, MAX_STRING_SIZE, "BLOCKED BY %s!", tName);
      ActionSummaryAddString(summary, temp, board, box, textType);
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
  for (int i = 0; i < action->numOtherTargetsKilled; i++) {
    Actor* otherT = action->otherTargetsKilled[i];
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
  if (summary->currentString == 0 || summary->currentString + 1 >= summary->numStrings) {
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


