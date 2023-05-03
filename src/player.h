#ifndef PLAYER_H
#define PLAYER_H

#include "actor.h"
#include "inventory.h"

#define MAX_PARTY_SIZE 4
#define MAX_NAME_SIZE 12

// Player character data
// Party members, inventory, etc.
typedef struct PlayerInfo {
  char name[MAX_NAME_SIZE];
  
  Actor** party;
  int partySize;

  Inventory* inventory;
} PlayerInfo;

PlayerInfo* playerInfoCreate(char* name,  COISprite* sprite, Inventory* inventory);
void playerInfoDestroy(PlayerInfo* info);

#endif
