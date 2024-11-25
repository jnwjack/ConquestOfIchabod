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
    tech->strength = 0.5;
    return tech;
  case TECH_ID_COUNTER:
    tech->cost = 7;
    tech->strength = 1.0;
    return tech;
  case TECH_ID_BRACE:
    tech->cost = 1;
    tech->strength = 0.5;
    return tech;
  case TECH_ID_RAGE:
    tech->cost = 5;
    tech->strength = 1.5;
    return tech;
  case TECH_ID_EAGLEEYE:
    tech->cost = 4;
    tech->strength = 1.0;
    return tech;
  case TECH_ID_QUICKSTRIKE:
    tech->cost = 6;
    tech->strength = 1.0;
    return tech;
  case TECH_ID_FADE:
    tech->cost = 5;
    tech->strength = 1.0;
    return tech;
  case TECH_ID_MAGIC_GUARD:
    tech->cost = 8;
    tech->strength = 1.0;
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

void techRemoveFromList(TechList* list, Tech* tech) {
  for (int i = 0; i < list->count; i++) {
    if (list->techs[i] == tech) {
      techDestroy(list->techs[i]);
      for (int j = i + 1; j < list->count; j++) {
        list->techs[j - 1] = list->techs[j];
      }
    }
    list->count--;
    break;
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
  case TECH_ID_EAGLEEYE:
    return "EagleEye";
  case TECH_ID_QUICKSTRIKE:
    return "Amplify";
  case TECH_ID_FADE:
    return "Fade";
  case TECH_ID_MAGIC_GUARD:
    return "Magic Guard";
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
  case TECH_ID_EAGLEEYE:
    return "Attacks won't miss.";
  case TECH_ID_QUICKSTRIKE:
    return "Damaging SPECIAL abilities activate twice.";
  case TECH_ID_FADE:
    return "Higher chance to dodge attacks.";
  case TECH_ID_MAGIC_GUARD:
    return "Receive no damage from SPECIAL abilities.";
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

