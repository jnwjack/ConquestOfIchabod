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
  case SPECIAL_ID_DRAIN_SPIRIT:
    return "Drain Spirit";
  case SPECIAL_ID_REINFORCE:
    return "Reinforce";
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
  case SPECIAL_ID_DRAIN_SPIRIT:
    return 5;
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
  case SPECIAL_ID_DRAIN_SPIRIT:
    return SPECIAL_DEBUFF;
  case SPECIAL_ID_PARRY:
  case SPECIAL_ID_REINFORCE:
    return SPECIAL_BUFF;
  default:
    printf("Invalid id for special type lookup.\n");
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
  case SPECIAL_ID_DRAIN_SPIRIT:
  case SPECIAL_ID_ICE_SPEAR:
    return 4;
  case SPECIAL_ID_HEAL:
  case SPECIAL_ID_CURSE:
    return 7;
  case SPECIAL_ID_PARRY:
    return 10;
  default:
    printf("Invalid special id for cost.\n");
    return -1;
  }
}

char* specialDescription(int id) {
  switch (id) {
  case SPECIAL_ID_FIREBALL:
    return "Deal high damage to a single target";
  case SPECIAL_ID_ICE_SPEAR:
    return "Deal moderate damage to multiple targets";
  case SPECIAL_ID_HEAL:
    return "Restore HP to a friendly target";
  case SPECIAL_ID_CURSE:
    return "Any healing effect applied to the target will instead deal damage";
  case SPECIAL_ID_PARRY:
    return "Block attack and deal damage to attacker";
  case SPECIAL_ID_DRAIN_SPIRIT:
    return "Reduce the target's SP";
  default:
    return "Error: Not implemented";
  }
}

