#include "special.h"

char* specialName(int id) {
  switch (id) {
  case SPECIAL_ID_FIREBALL:
    return "Fireball";
  case SPECIAL_ID_ICE_SPEAR:
    return "Blitz Fire";
  case SPECIAL_ID_AVALANCHE:
    return "Avalanche";
  case SPECIAL_ID_HEAL:
    return "Heal";
  case SPECIAL_ID_MINOR_HEAL:
    return "Minor Heal";
  case SPECIAL_ID_PARRY:
    return "Parry";
  case SPECIAL_ID_CURSE:
    return "Curse";
  case SPECIAL_ID_DRAIN_SPIRIT:
    return "Drain Spirit";
  case SPECIAL_ID_REINFORCE:
    return "Reinforce";
  case SPECIAL_ID_HOWL:
    return "Howl";
  case SPECIAL_ID_NEUTRALIZE:
    return "Neutralize";
  case SPECIAL_ID_HASTE:
    return "Haste";
  case SPECIAL_ID_SILENCE:
    return "Silence";
  case SPECIAL_ID_DAYDREAM:
    return "Daydream";
  case SPECIAL_ID_BAG_GROCERIES:
    return "Bag Items";
  case SPECIAL_ID_SLACK_OFF:
    return "Slack Off";
  case SPECIAL_ID_STOCK_SHELVES:
    return "Stock Goods";
  case SPECIAL_ID_CLEAN:
    return "Clean";
  case SPECIAL_ID_WATCH_CLOCK:
    return "Watch Clock";
  case SPECIAL_ID_BACKSTAB:
    return "Backstab";
  case SPECIAL_ID_TIME_SKIP:
    return "Time Skip";
  case SPECIAL_ID_FIRE_BOLT:
    return "Fire Bolt";
  }

  return "ERROR";
}

char* specialVerb(int id) {
  switch (id) {
  case SPECIAL_ID_PARRY:
    return "READIES";
  case SPECIAL_ID_HOWL:
  case SPECIAL_ID_BACKSTAB:
    return "USES";
  case SPECIAL_ID_DAYDREAM:
  case SPECIAL_ID_BAG_GROCERIES:
  case SPECIAL_ID_SLACK_OFF:
  case SPECIAL_ID_STOCK_SHELVES:
  case SPECIAL_ID_CLEAN:
  case SPECIAL_ID_WATCH_CLOCK:
    return "WASTES TIME WITH";
  default:
    return "CASTS";
  }
}

int specialStrength(int id) {
  switch (id) {
  case SPECIAL_ID_FIREBALL:
    return 35;
  case SPECIAL_ID_AVALANCHE:
    return 15;
  case SPECIAL_ID_ICE_SPEAR:
    return 18;
  case SPECIAL_ID_FIRE_BOLT:
    return 10;
  case SPECIAL_ID_HEAL:
    return 30;
  case SPECIAL_ID_MINOR_HEAL:
    return 15;
  case SPECIAL_ID_DRAIN_SPIRIT:
    return 5;
  default:
    return 0;
  }
}


SpecialType specialType(int id) {
  switch (id) {
  case SPECIAL_ID_ICE_SPEAR:
  case SPECIAL_ID_FIREBALL:
  case SPECIAL_ID_AVALANCHE:
  case SPECIAL_ID_BACKSTAB:
  case SPECIAL_ID_FIRE_BOLT:
    return SPECIAL_DAMAGING;
  case SPECIAL_ID_HEAL:
  case SPECIAL_ID_MINOR_HEAL:
    return SPECIAL_HEALING;
  case SPECIAL_ID_CURSE:
  case SPECIAL_ID_DRAIN_SPIRIT:
  case SPECIAL_ID_NEUTRALIZE:
  case SPECIAL_ID_SILENCE:
    return SPECIAL_DEBUFF;
  case SPECIAL_ID_PARRY:
  case SPECIAL_ID_REINFORCE:
  case SPECIAL_ID_HOWL:
  case SPECIAL_ID_HASTE:
  case SPECIAL_ID_TIME_SKIP:
    return SPECIAL_BUFF;
  case SPECIAL_ID_DAYDREAM:
  case SPECIAL_ID_BAG_GROCERIES:
  case SPECIAL_ID_SLACK_OFF:
  case SPECIAL_ID_STOCK_SHELVES:
  case SPECIAL_ID_CLEAN:
  case SPECIAL_ID_WATCH_CLOCK:
    return SPECIAL_GAG;
  default:
    printf("Invalid id for special type lookup.\n");
    return SPECIAL_DAMAGING;
  }
}

bool specialTargetsSelf(int id) {
  return id == SPECIAL_ID_HOWL;
}

bool specialTargetsEnemies(int id) {
  SpecialType type = specialType(id);
  return type == SPECIAL_DAMAGING || type == SPECIAL_DEBUFF;
}

int specialCost(int id) {
  switch (id) {
  case SPECIAL_ID_FIREBALL:
  case SPECIAL_ID_AVALANCHE:
    return 8;
  case SPECIAL_ID_DRAIN_SPIRIT:
  case SPECIAL_ID_ICE_SPEAR:
  case SPECIAL_ID_NEUTRALIZE:
    return 4;
  case SPECIAL_ID_HEAL:
  case SPECIAL_ID_MINOR_HEAL:
  case SPECIAL_ID_CURSE:
    return 7;
  case SPECIAL_ID_PARRY:
    return 5;
  case SPECIAL_ID_BACKSTAB:
    return 6;
  case SPECIAL_ID_HASTE:
    return 5;
  case SPECIAL_ID_DAYDREAM:
  case SPECIAL_ID_BAG_GROCERIES:
  case SPECIAL_ID_SLACK_OFF:
  case SPECIAL_ID_STOCK_SHELVES:
  case SPECIAL_ID_CLEAN:
  case SPECIAL_ID_WATCH_CLOCK:
    return 1;
  case SPECIAL_ID_TIME_SKIP:
    return 0;
  default:
    printf("Invalid special id for cost.\n");
    return 0;
  }
}

char* specialDescription(int id) {
  switch (id) {
  case SPECIAL_ID_FIREBALL:
    return "Deal high damage to a single target";
  case SPECIAL_ID_ICE_SPEAR:
    return "Deal moderate damage to a single target";
  case SPECIAL_ID_AVALANCHE:
    return "Deal low damage to multiple targets";
  case SPECIAL_ID_MINOR_HEAL:
  case SPECIAL_ID_HEAL:
    return "Restore HP to a friendly target";
  case SPECIAL_ID_CURSE:
    return "Any healing effect applied to the target will instead deal damage";
  case SPECIAL_ID_PARRY:
    return "Block attack and deal damage to attacker";
  case SPECIAL_ID_DRAIN_SPIRIT:
    return "Reduce the target's SP";
  case SPECIAL_ID_SILENCE:
    return "Prevent the target from using SPECIAL for five turns";
  case SPECIAL_ID_DAYDREAM:
    return "Is this how you thought things would be going?";
  case SPECIAL_ID_BAG_GROCERIES:
    return "Pack up the customer's purchases. Bread on the top.";
  case SPECIAL_ID_SLACK_OFF:
    return "Fritter and waste the hours in an offhand way.";
  case SPECIAL_ID_STOCK_SHELVES:
    return "Lift cans and boxes onto display shelves. This is your main source of excercise.";
  case SPECIAL_ID_CLEAN:
    return "Wipe down the counter and any other messes.";
  case SPECIAL_ID_WATCH_CLOCK:
    return "What's a little more wasted time at this point?";
  case SPECIAL_ID_BACKSTAB:
    return "Deal damage to enemy. The higher the user's AGI is compared to the target's, the more damage is dealt.";
  case SPECIAL_ID_HASTE:
    return "Increase AGI";
  case SPECIAL_ID_TIME_SKIP:
    return "Escape battle by moving forward in time.";
  case SPECIAL_ID_NEUTRALIZE:
    return "Remove modifiers to target's stats.";
  default:
    return "Error: Not implemented";
  }
}

// Strength value for spells that apply a secondary, weaker effect. Ex: Avalanche
int specialSecondaryStrength(int id) {
  return specialStrength(id) * 1;
}
