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

  // Modifiers
  float attackModifier; // Effectiveness modifier from actor/target
  float spCostModifier;
  bool damageAttacker;
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
Actor* battleBehaviorPickTarget(int actorType, ActionType action, Actor** enemies, int numEnemies, Actor** allies, int numAllies);
int battleBehaviorPickIndex(ActionType action, TechList* techList);
BattleAction battleBehaviorGenerateAction(Actor* actor, Actor** actorEnemies, int numEnemies, Actor** actorAllies, int numAllies);

// Sorting
void battleBehaviorSwapActions(BattleAction* a, BattleAction* b);
void battleBehaviorSortActions(BattleAction* actions, int numActions);

ActionSummary* battleBehaviorDoAction(BattleAction* action, char* playerName, COITextType* textType, COIBoard* board, COISprite* box);

ActionSummary* ActionSummaryCreate(COIBoard* board, COISprite* box, COITextType* textType, char* string, ...);
void ActionSummaryAdvance(ActionSummary* summary, bool skipToNextString);
void ActionSummaryPosition(ActionSummary* summary, int x, int y);
//void ActionSummaryAddString(ActionSummary* summary, char* newString);
void ActionSummaryAddString(ActionSummary* summary, char* newString, COIBoard* board, COISprite* sprite, COITextType* textType);
void ActionSummaryDestroy(ActionSummary* summary, COIBoard* board);


#endif
