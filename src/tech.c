#include "tech.h"


// Constructors --------------------------
Tech* techCreate(int id) {
  Tech* tech = malloc(sizeof(Tech));
  tech->id = id;
  tech->active = false;
  tech->consecutive = 0;
  
  switch (id) {
  case TECH_ID_FOCUS:
    tech->cost = 3;
    tech->strength = 5;
    return tech;
  case TECH_ID_COUNTER:
    tech->cost = 7;
    tech->strength = 10;
    return tech;
  case TECH_ID_BRACE:
    tech->cost = 1;
    tech->strength = 4;
    return tech;
  case TECH_ID_RAGE:
    tech->cost = 5;
    tech->strength = 12;
    return tech;
  }

  return tech;
}

TechList* techCreateList(int maxCount) {
  TechList* list = malloc(sizeof(TechList));
  list->techs = malloc(sizeof(Tech*) * maxCount);
  list->count = 0;
  list->maxCount = maxCount;

  return list;
}
//----------------------------------------------

COIString* techNameAsCOIString(Tech* tech, int x, int y, COITextType* textType, bool asterisk) {
  char name[MAX_NAME_SIZE];
  // Include asterisk at end? Used when indicating a tech is active
  if (asterisk) {
    snprintf(name, MAX_NAME_SIZE, "%s *", techNameFromID(tech->id));
  } else {
    snprintf(name, MAX_NAME_SIZE, "%s", techNameFromID(tech->id));
  }
  
  return COIStringCreate(name, x, y, textType);
}

void techAddToList(TechList* list, int id) {
  if (list->count < list->maxCount) {
    list->techs[list->count] = techCreate(id);
    list->count++;
  } else {
    printf("Error: TECH list full.\n");
  }
}

char* techNameFromID(int id) {
  switch (id) {
  case TECH_ID_FOCUS:
    return "Focus";
  case TECH_ID_COUNTER:
    return "Counter";
  case TECH_ID_BRACE:
    return "Brace";
  case TECH_ID_RAGE:
    return "Rage";
  }

  return "ERROR";
}

char* techDescFromID(int id) {
switch (id) {
  case TECH_ID_FOCUS:
    return "SP skills costs less.";
  case TECH_ID_COUNTER:
    return "Hurt attacker when hit.";
  case TECH_ID_BRACE:
    return "Reduce damage taken.";
  case TECH_ID_RAGE:
    return "Take more damage, deal more damage.";
  }

  return "ERROR";
}

// Destructors ---------------------------------
void techDestroy(Tech* tech) {
  free(tech);
}
void techDestroyList(TechList* list) {
  for (int i = 0; i < list->count; i++) {
    techDestroy(list->techs[i]);
  }

  free(list->techs);
  free(list);
}
// ---------------------------------------------

