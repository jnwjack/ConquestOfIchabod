#ifndef TECH_H
#define TECH_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "constants.h"
#include "engine/COI.h"


// Holds data about a technique (TECH)
// Actor holds an array of these.

// TECH IDs -----------------------------
#define TECH_ID_FOCUS 0 // SP skills costs less
#define TECH_ID_COUNTER 1 // Hurt attacker when hit
#define TECH_ID_BRACE 2 // Reduce damage taken
#define TECH_ID_RAGE 3 // Take more damage, deal more damage
#define TECH_ID_EAGLEEYE 4 // Can't miss
#define TECH_ID_QUICKSTRIKE 5 // Damaging specials activate twice.
#define TECH_ID_FADE 6 // Increased dodge chance
#define TECH_ID_MAGIC_GUARD 7 // No damage from specials
// --------------------------------------

#define MAX_TECH_COUNT_ALLY 50
#define MAX_TECH_COUNT_NPC 5

typedef struct Tech {
  int id;
  int cost; // Cost per turn to apply tech
  float strength; // Strength of effect
  int consecutive;
  bool active;
} Tech;

typedef struct TechList {
  Tech** techs;
  int count;
  int maxCount;
} TechList;

Tech* techCreate(int id);
TechList* techCreateList(int maxCount);
void techDestroy(Tech* tech);
void techDestroyList(TechList* list);

void techAddToList(TechList* list, int id);
void techRemoveFromList(TechList* list, Tech* tech);
char* techNameFromID(int id);
char* techDescFromID(int id);

COIString* techNameAsCOIStringWithCost(Tech* tech, int x, int y, COITextType* textType, bool asterisk);
COIString* techNameAsCOIString(Tech* tech, int x, int y, COITextType* textType);


#endif

