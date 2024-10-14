#ifndef SPECIAL_H
#define SPECIAL_H

#include <stdbool.h>
#include <stdio.h>
#include "engine/COI.h"

/*
JNW: Maybe do something like this in the future

typedef enum {
  SPECIAL_TYPE_WATER,
  SPECIAL_TYPE_FIRE,
  SPECIAL_TYPE_DARK,
  SPECIAL_TYPE_LIGHT,
  SPECIAL_TYPE_PHYSICAL
} SpecialElement;

typedef enum {
}

typedef struct Special {
  SpecialType type;
  int id;
} Special;

extern Special GLOBAL_SPECIALS;

Special* SPECIAL_FROM_ID(int id);

*/

#define SPECIAL_ID_FIREBALL 0 // Attack
#define SPECIAL_ID_ICE_SPEAR 1 // Attack
#define SPECIAL_ID_HEAL 2 // Healing
#define SPECIAL_ID_PARRY 3 // Massive damage when attacked
#define SPECIAL_ID_CURSE 4 // Causes damage when they attempt to heal
#define SPECIAL_ID_REVIVE 5 // Revive dead enemies
#define SPECIAL_ID_DISTRACT 6 // Target's next move must target actor
#define SPECIAL_ID_DRAIN_SPIRIT 7 // Drain target's TP or SP
#define SPECIAL_ID_REINFORCE 8
#define SPECIAL_ID_AVALANCHE 9
#define SPECIAL_ID_HOWL 10 // Actor and everyone nearby has boosted AGI.
#define SPECIAL_ID_NEUTRALIZE 11 // Return all stat changes on target to 1.0.
#define SPECIAL_ID_HASTE 12
#define SPECIAL_ID_SILENCE 13 // Target can't use specials for a bit
#define SPECIAL_ID_BACKSTAB 14 // Deal damage based on AGI
#define SPECIAL_ID_TIME_SKIP 21

// These are for a gag and don't do anything
#define SPECIAL_ID_DAYDREAM 15
#define SPECIAL_ID_BAG_GROCERIES 16
#define SPECIAL_ID_SLACK_OFF 17
#define SPECIAL_ID_STOCK_SHELVES 18
#define SPECIAL_ID_CLEAN 19
#define SPECIAL_ID_WATCH_CLOCK 20

typedef enum {
  SPECIAL_DAMAGING,
  SPECIAL_HEALING,
  SPECIAL_BUFF,
  SPECIAL_DEBUFF,
  SPECIAL_GAG,
} SpecialType;

char* specialName(int id);
bool specialTargetsEnemies(int id);
bool specialTargetsSelf(int id);
SpecialType specialType(int id);
char* specialVerb(int id);
int specialStrength(int id);
int specialCost(int id);
char* specialDescription(int id);
int specialSecondaryStrength(int id);

#endif
