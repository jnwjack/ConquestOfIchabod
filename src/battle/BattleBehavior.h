#ifndef BATTLEBEHAVIOR_H
#define BATTLEBEHAVIOR_H

#include "../player.h"

#define BB_DEFAULT_HIT_RATE 0.9f
#define BB_FADE_HIT_RATE 0.65f

#define BB_MAX_ENEMY_COUNT 5

typedef enum {
  ATTACK,
  SPECIAL,
  ITEM,
  FLEE,
  INACTIVE
} ActionType;

typedef enum ModifierType {
  MT_PARRYING,
  MT_CURSED,
  MT_SILENCED
} ModifierType;

typedef struct ActorBattleModifier {
  ModifierType type;
  unsigned int turnsLeft;
  Actor* actor;
} ActorBattleModifier;

typedef struct BattleAction {
  Actor* actor; // Who is acting?
  Actor* target; // Targeted actor of attack/special/item
  ActionType type;
  int index; // Index of used special/item

  // Modifiers
  float attackModifier; // Effectiveness modifier from actor/target
  float spCostModifier;
  bool damageAttacker;
  bool successfulFlee;
  int numOtherTargets;
  Actor* otherTargets[BB_MAX_ENEMY_COUNT];
} BattleAction;

// Data structure that contains text that describes an
// action that just took place. Used for populating
// text box in battles.
typedef struct ActionSummary {
  LinkedList* strings; // Should use a vector instead
  COIString* currentStringObj;
  //COIString** strings;
  int numStrings;
  int currentString;
  int ticksPerString;
  int ticks;
  bool finished;
} ActionSummary;

// AI action selection
ActionType battleBehaviorPickActionType(int actorType);
Actor* battleBehaviorPickTarget(int actorType, BattleAction* action, Actor** enemies, int numEnemies, Actor** allies, int numAllies);
int battleBehaviorPickIndex(ActionType action, Actor* actor);
void battleBehaviorGenerateAction(BattleAction* action, Actor* actor, Actor** actorEnemies, int numEnemies, Actor** actorAllies, int numAllies);

void battleBehaviorMakeSpecial(BattleAction* action, int specialID, int targetIndex, Actor** targetActors, int numTargetActors, Actor* actor);

// Sorting
void battleBehaviorSwapActions(BattleAction* a, BattleAction* b);
void battleBehaviorSortActions(BattleAction* actions, int numActions);

ActionSummary* battleBehaviorDoAction(BattleAction* action, COITextType* textType, COIBoard* board, COISprite* box, PlayerInfo* pInfo, LinkedList* modifiers);
void battleBehaviorUpdateModifiersTimeLeft(LinkedList* modifiers);
bool battleBehaviorCheckForModifiers(Actor* actor, ModifierType type, LinkedList* modifiers);
void battleBehaviorsDestroyModifiers(LinkedList* modifiers);

ActionSummary* ActionSummaryCreate(COIBoard* board, COISprite* box, COITextType* textType, char* string, ...);
void ActionSummaryAdvance(ActionSummary* summary, bool skipToNextString);
void ActionSummaryPosition(ActionSummary* summary, int x, int y);
//void ActionSummaryAddString(ActionSummary* summary, char* newString);
void ActionSummaryAddString(ActionSummary* summary, char* newString, COIBoard* board, COISprite* sprite, COITextType* textType);
void ActionSummaryDestroy(ActionSummary* summary, COIBoard* board);


#endif
