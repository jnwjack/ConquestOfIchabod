#include "player.h"
#include "special.h"

static unsigned int LEVELUP_TECHS_FIGHTER[] = { TECH_ID_FOCUS, TECH_ID_COUNTER, TECH_ID_BRACE, TECH_ID_RAGE };
static unsigned int LEVELUP_SPECIALS_FIGHTER[] = { SPECIAL_ID_PARRY };
static unsigned int LEVELUP_TECHS_MIN_LEVELS[] = { 1, 5, 7, 11 };
static unsigned int LEVELUP_SPECIALS_MIN_LEVELS[] = { 1 };
#define LEVELUP_NUM_TECHS_FIGHTER 4
#define LEVELUP_NUM_SPECIALS_FIGHTER 1

PlayerInfo* playerInfoCreate(char* name,  COISprite* sprite, Inventory* inventory) {
  PlayerInfo* info = malloc(sizeof(PlayerInfo));

  info->inventory = inventory;
  info->party = malloc(sizeof(Actor*) * MAX_PARTY_SIZE);
  info->party[0] = actorCreatePlayer(sprite);
  info->partySize = 1;
  info->level = 1;
  info->xp = 0;
  info->xpForLevelUp = 50;
  info->renting = false;
  info->working = false;
  info->alreadyHealed = false;
  info->nextRentDate = 30;
  info->classProgression.specialsIndex = 0;
  info->classProgression.techsIndex = 0;
  // Should change based on class type
  info->classProgression.specials = LEVELUP_SPECIALS_FIGHTER;
  info->classProgression.techs = LEVELUP_TECHS_FIGHTER;
  info->classProgression.numTechs = LEVELUP_NUM_TECHS_FIGHTER;
  info->classProgression.numSpecials = LEVELUP_NUM_SPECIALS_FIGHTER;
  info->classProgression.specialsLevels = LEVELUP_SPECIALS_MIN_LEVELS;
  info->classProgression.techsLevels = LEVELUP_TECHS_MIN_LEVELS;

  // Copy name from argument
  int nameIndex = 0;
  char c = name[nameIndex];
  while (c != '\0' && nameIndex < MAX_NAME_SIZE - 1) {
    info->name[nameIndex] = c;
    nameIndex++;
    c = name[nameIndex];
  }
  info->name[nameIndex] = '\0';

  return info;
}

unsigned long _getXPForLevel(unsigned long oldXP) {
  return oldXP * 1.5;
}

void playerAddXP(PlayerInfo* info, unsigned long xp) {
  info->xp += xp;
  if (info->xp >= info->xpForLevelUp) {
    info->level++;
    info->xp -= info->xpForLevelUp;
    info->xpForLevelUp = _getXPForLevel(info->xpForLevelUp);
  }
}

int playerAdjustedATK(PlayerInfo* info) {
  Actor* player = info->party[0];
  Inventory* inventory = info->inventory;
  return ItemAdjustStat(actorModifiedAtk(player), inventory->weapon->strength);
}

int playerAdjustedDEF(PlayerInfo* info) {
  Actor* player = info->party[0];
  Inventory* inventory = info->inventory;
  int defItemTotal = inventoryDEFItemTotalStrength(inventory);
  return ItemAdjustStat(player->atk, defItemTotal);
}

int playerAdjustedAGI(PlayerInfo* info) {
  Actor* player = info->party[0];
  return player->agi;
}

int playerAdjustedHP(PlayerInfo* info) {
  Actor* player = info->party[0];
  return player->hp;
}

int playerAdjustedSP(PlayerInfo* info) {
  Actor* player = info->party[0];
  return player->sp;
}

int playerAdjustedTP(PlayerInfo* info) {
  Actor* player = info->party[0];
  return player->tp;
}

// Returns one of the possible abilities the player gets upon level up.
Tech* playerGetNextTech(PlayerInfo* info) {
  int id = info->classProgression.techs[info->classProgression.techsIndex];
  return techCreate(id);
}

int playerGetNextSpecial(PlayerInfo* info) {
  return info->classProgression.specials[info->classProgression.specialsIndex];
}

bool playerHasValidNextTech(PlayerInfo* pInfo) {
  return (pInfo->classProgression.techsIndex < pInfo->classProgression.numTechs &&
          pInfo->level >= pInfo->classProgression.techsLevels[pInfo->classProgression.techsIndex]);

}

bool playerHasValidNextSpecials(PlayerInfo* pInfo) {
  return (pInfo->classProgression.specialsIndex < pInfo->classProgression.numSpecials &&
        pInfo->level >= pInfo->classProgression.specialsLevels[pInfo->classProgression.specialsIndex]);
}


void playerInfoDestroy(PlayerInfo* info) {
  inventoryDestroy(info->inventory);
  for (int i = 0; i < info->partySize; i++) {
    actorDestroy(info->party[i]);
  }
  free(info->party);
  free(info);
}
