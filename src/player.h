#ifndef PLAYER_H
#define PLAYER_H

#include "actor.h"
#include "inventory.h"
#include "constants.h"

#define MAX_PARTY_SIZE 4

typedef struct ClassProgression {
  unsigned int* techs;
  unsigned int* specials;

  // Minimum levels needed to acquire each skill in list
  unsigned int* techsLevels;
  unsigned int* specialsLevels;

  unsigned int techsIndex;
  unsigned int specialsIndex;
  unsigned int numTechs;
  unsigned int numSpecials;
} ClassProgression;

// Player character data
// Party members, inventory, etc.
typedef struct PlayerInfo {
  char name[MAX_NAME_SIZE];

  bool renting;
  bool working;
  bool alreadyHealed; // Can only heal once per rest via house.
  unsigned long nextRentDate;
  
  Actor** party;
  int partySize;

  Inventory* inventory;

  unsigned int level;
  unsigned long xp;
  unsigned long xpForLevelUp;

  ClassProgression classProgression;
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

bool playerHasValidNextTech(PlayerInfo* pInfo);
bool playerHasValidNextSpecials(PlayerInfo* pInfo);

Tech* playerGetNextTech(PlayerInfo* info);
int playerGetNextSpecial(PlayerInfo* info);

void playerEncode(PlayerInfo* info);
PlayerInfo* playerDecode(ItemList* items, COISprite* playerSprite, Inventory* inventory);
bool playerSaveExists();

#endif
