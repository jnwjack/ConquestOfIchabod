#ifndef PLAYER_H
#define PLAYER_H

#include "actor.h"
#include "inventory.h"
#include "constants.h"

#define MAX_PARTY_SIZE 4

#define PLAYER_CLASS_FIGHTER 0
#define PLAYER_CLASS_WIZARD 1
#define PLAYER_CLASS_ROGUE 2

typedef enum RentStatus {
  RS_RENTING = 0,
  RS_NOT_RENTING,
  RS_EVICTED
} RentStatus;

typedef enum SpriteAge {
  SA_YOUNG = 0,
  SA_OLDER,
  SA_OLDEST
} SpriteAge;

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
  int class;

  RentStatus renting;
  SpriteAge spriteAge;
  bool working;
  bool alreadyHealed; // Can only heal once per rest via house.
  unsigned long nextRentDate;

  // Weird happenings
  bool rentHouseBaldUsed;
  unsigned int shiftsWorked;
  TimeState lastXPGain;
  bool foundMythicalSword;
  bool armoryGagUsed;
  
  Actor** party;
  int partySize;

  Inventory* inventory;

  unsigned int level;
  unsigned long xp;
  unsigned long xpForLevelUp;

  ClassProgression classProgression;
} PlayerInfo;

void playerAddXP(PlayerInfo* info, unsigned long xp);
void playerLevelDown(PlayerInfo* pInfo);
PlayerInfo* playerInfoCreate(char* name,  COISprite* sprite, Inventory* inventory, int class);
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

char* playerGetClass(PlayerInfo* pInfo);
char* playerClassNameFromID(int id);

void playerCheckForEviction(PlayerInfo* pInfo);

SpriteAge playerSpriteAgeFromGlobalTime();
int playerSpriteIndexFromSpriteAge(SpriteAge age);
void playerUpdateClassProgressionFromTime(PlayerInfo* pInfo);

#endif
