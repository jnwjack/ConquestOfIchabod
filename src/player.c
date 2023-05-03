#include "player.h"

PlayerInfo* playerInfoCreate(char* name,  COISprite* sprite, Inventory* inventory) {
  PlayerInfo* info = malloc(sizeof(PlayerInfo));

  info->inventory = inventory;
  info->party = malloc(sizeof(Actor*) * MAX_PARTY_SIZE);
  info->party[0] = actorCreatePlayer(sprite);
  info->partySize = 1;

  // Copy name from argument
  char c = name[0];
  info->partySize = 0;
  while (c != '\0') {
    info->name[info->partySize] = c;
    info->partySize++;
    c = name[info->partySize];
  }

  return info;
}

void playerInfoDestroy(PlayerInfo* info) {
  inventoryDestroy(info->inventory);
  for (int i = 0; i < info->partySize; i++) {
    actorDestroy(info->party[i]);
  }
  free(info->party);
  free(info);
}
