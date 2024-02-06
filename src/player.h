#ifndef PLAYER_H
#define PLAYER_H

#include "actor.h"
#include "inventory.h"
#include "constants.h"

#define MAX_PARTY_SIZE 4

// Player character data
// Party members, inventory, etc.
typedef struct PlayerInfo {
  char name[MAX_NAME_SIZE];

  bool alreadyHealed; // Can only heal once per rest via house.
  
  Actor** party;
  int partySize;

  Inventory* inventory;

  unsigned int level;
  unsigned long xp;
  unsigned long xpForLevelUp;
} PlayerInfo;

void playerAddXP(PlayerInfo* info, unsigned long xp);
PlayerInfo* playerInfoCreate(char* name,  COISprite* sprite, Inventory* inventory);
void playerInfoDestroy(PlayerInfo* info);

// Stat values affected by gear
int playerAdjustedATK(PlayerInfo* info);
int playerAdjustedDEF(PlayerInfo* info);
int playerAdjustedAGI(PlayerInfo* info);
int playerAdjustedHP(PlayerInfo* info);
int playerAdjustedSP(PlayerInfo* info);
int playerAdjustedTP(PlayerInfo* info);

#endif
