#include "special.h"

char* specialName(int id) {
  switch (id) {
  case SPECIAL_ID_FIREBALL:
    return "Fireball";
  case SPECIAL_ID_ICE_SPEAR:
    return "Ice Spear";
  case SPECIAL_ID_HEAL:
    return "Heal";
  case SPECIAL_ID_PARRY:
    return "Parry";
  case SPECIAL_ID_CURSE:
    return "Curse";
  }

  return "ERROR";
}

char* specialVerb(int id) {
  switch (id) {
  case SPECIAL_ID_PARRY:
    return "READIES";
  default:
    return "CASTS";
  }
}

int specialStrength(int id) {
  switch (id) {
  case SPECIAL_ID_FIREBALL:
    return 10;
  case SPECIAL_ID_ICE_SPEAR:
    return 8;
  case SPECIAL_ID_HEAL:
    return 12;
  default:
    return -1;
  }
}


SpecialType specialType(int id) {
  switch (id) {
  case SPECIAL_ID_ICE_SPEAR:
  case SPECIAL_ID_FIREBALL:
    return SPECIAL_DAMAGING;
  case SPECIAL_ID_HEAL:
    return SPECIAL_HEALING;
  case SPECIAL_ID_CURSE:
  case SPECIAL_ID_PARRY:
    return SPECIAL_DEBUFF;
  default:
    printf("Invalid special type.\n");
    return SPECIAL_DAMAGING;
  }
}

bool specialTargetsEnemies(int id) {
  SpecialType type = specialType(id);
  return type == SPECIAL_DAMAGING || type == SPECIAL_DEBUFF;
}

int specialCost(int id) {
  switch (id) {
  case SPECIAL_ID_FIREBALL:
    return 8;
  case SPECIAL_ID_ICE_SPEAR:
    return 4;
  case SPECIAL_ID_HEAL:
  case SPECIAL_ID_CURSE:
    return 7;
  case SPECIAL_ID_PARRY:
    return 10;
  }
}

