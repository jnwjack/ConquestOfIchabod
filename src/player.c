#include "player.h"

PlayerInfo* playerInfoCreate(char* name,  COISprite* sprite, Inventory* inventory) {
  PlayerInfo* info = malloc(sizeof(PlayerInfo));

  info->inventory = inventory;
  info->party = malloc(sizeof(Actor*) * MAX_PARTY_SIZE);
  info->party[0] = actorCreatePlayer(sprite);
  info->partySize = 1;

  // Copy name from argument
  int nameIndex = 0;
  char c = name[nameIndex];
  while (c != '\0') {
    info->name[nameIndex] = c;
    nameIndex++;
    c = name[nameIndex];
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
