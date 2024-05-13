#ifndef SPECIAL_H
#define SPECIAL_H

#include <stdbool.h>
#include <stdio.h>

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
#define SPEICAL_ID_DRAIN_SPIRIT 7 // Drain target's TP or SP

typedef enum {
  SPECIAL_DAMAGING,
  SPECIAL_HEALING,
  SPECIAL_BUFF,
  SPECIAL_DEBUFF
} SpecialType;

char* specialName(int id);
bool specialTargetsEnemies(int id);
SpecialType specialType(int id);
char* specialVerb(int id);
int specialStrength(int id);
int specialCost(int id);
char* specialDescription(int id);

#endif
