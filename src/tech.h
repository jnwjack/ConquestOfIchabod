#ifndef TECH_H
#define TECH_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "constants.h"
#include "engine/COIString.h"


// Holds data about a technique (TECH)
// Actor holds an array of these.

// TECH IDs -----------------------------
#define TECH_ID_FOCUS 0 // SP skills costs less
#define TECH_ID_COUNTER 1 // Hurt attacker when hit
#define TECH_ID_BRACE 2 // Reduce damage taken
#define TECH_ID_RAGE 3 // Take more damage, deal more damage
// --------------------------------------

#define MAX_TECH_COUNT_ALLY 50
#define MAX_TECH_COUNT_NPC 5

typedef struct Tech {
  int id;
  int cost; // Cost per turn to apply tech
  int strength; // Strength of effect
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
char* techNameFromID(int id);
char* techDescFromID(int id);

COIString* techNameAsCOIString(Tech* tech, int x, int y, COITextType* textType, bool asterisk);


#endif

