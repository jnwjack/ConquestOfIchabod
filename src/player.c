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

char _rotateChar(unsigned char c, int shift) {
  if (c == '\n') {
    return '\0';
  }

  unsigned char normalized = c - '0';
  unsigned char offset = normalized + shift;
  const unsigned char numVals = 'z' - '0';
  return (char)((offset % numVals) + '0');
}

void _encodeString(char* string, FILE* fp) {
  size_t len = strnlen(string, MAX_STRING_SIZE);
  for (int i = 0; i < len; i++) {
    fputc(_rotateChar((unsigned char)string[i], 15), fp);
  }
  fputc('\n', fp);
}

void _decodeString(char* string, char* output) {
  size_t len = strnlen(string, MAX_STRING_SIZE);
  for (int i = 0; i < len; i++) {
    output[i] = _rotateChar((unsigned char)string[i], 59);
  }
  output[len] = '\0';
}

void _encodeInt(int val, char* buf, FILE* fp) {
  SDL_itoa(val, buf, 10);
  _encodeString(buf, fp);
}

void _encodeULong(unsigned long val, char* buf, FILE* fp) {
  SDL_ultoa(val, buf, 10);
  _encodeString(buf, fp);
}

int _decodeInt(char** line, size_t* len, FILE* fp, char* buf) {
  getline(line, len, fp);
  _decodeString(*line, buf);
  return SDL_atoi(buf);
}

unsigned long _decodeULong(char** line, size_t* len, FILE* fp, char* buf) {
  getline(line, len, fp);
  _decodeString(*line, buf);
  unsigned long res = SDL_strtoul(buf, NULL, 10);
  return res;
}

void playerEncode(PlayerInfo* info) {
  FILE* fp = fopen("src/engine/etc/save.dat", "w");

  _encodeString(info->name, fp);

  char temp[MAX_STRING_SIZE];

  // Stats
  _encodeInt(info->party[0]->atk, temp, fp);
  _encodeInt(info->party[0]->def, temp, fp);
  _encodeInt(info->party[0]->agi, temp, fp);
  _encodeInt(info->party[0]->hp, temp, fp);
  _encodeInt(info->party[0]->hpMax, temp, fp);
  _encodeInt(info->party[0]->sp, temp, fp);
  _encodeInt(info->party[0]->spMax, temp, fp);
  _encodeInt(info->party[0]->tp, temp, fp);
  _encodeInt(info->party[0]->tpMax, temp, fp);
  _encodeInt(info->level, temp, fp);
  
  _encodeULong(info->xp, temp, fp);
  _encodeULong(info->xpForLevelUp, temp, fp);
  _encodeULong(info->nextRentDate, temp, fp);
  printf("days left: %lu\n", info->nextRentDate);
  _encodeInt((int)info->renting, temp, fp);
  _encodeInt((int)info->working, temp, fp);
  _encodeInt((int)info->alreadyHealed, temp, fp);

  // Inventory
  _encodeInt(info->inventory->weapon->id, temp, fp);
  _encodeInt(info->inventory->offHand->id, temp, fp);
  _encodeInt(info->inventory->head->id, temp, fp);
  _encodeInt(info->inventory->body->id, temp, fp);
  _encodeInt(info->inventory->legs->id, temp, fp);
  _encodeInt(info->inventory->numEquippedItems, temp, fp);
  _encodeInt(info->inventory->numBackpackItems, temp, fp);
  for (int i = 0; i < info->inventory->numBackpackItems; i++) {
    _encodeInt(info->inventory->backpack[i]->id, temp, fp);
  }
  _encodeInt(info->inventory->money, temp, fp);

  // Time State
  _encodeInt((int)GLOBAL_TIME.val, temp, fp);
  _encodeULong(GLOBAL_TIME.day, temp, fp);
  _encodeInt((int)GLOBAL_TIME.phase, temp, fp);

  // Abilities
  _encodeInt(info->party[0]->techList->count, temp, fp);
  for (int i = 0; i < info->party[0]->techList->count; i++) {
    Tech* tech = info->party[0]->techList->techs[i];
    _encodeInt(info->party[0]->techList->techs[i]->id, temp, fp);
  }
  _encodeInt(info->party[0]->specials.length, temp, fp);
  for (int i = 0; i < info->party[0]->specials.length; i++) {
    _encodeInt(info->party[0]->specials.values[i], temp, fp);
  }

  // Player position
  _encodeInt(info->party[0]->sprite->_x, temp, fp);
  _encodeInt(info->party[0]->sprite->_y, temp, fp);


  fclose(fp);
}

PlayerInfo* playerDecode(ItemList* items, COISprite* playerSprite, Inventory* inventory) {
  FILE* fp = fopen("src/engine/etc/save.dat", "r");

  char* line = NULL;
  char buf[MAX_STRING_SIZE];
  char name[MAX_STRING_SIZE];
  size_t len = 0;

  getline(&line, &len, fp);
  _decodeString(line, name);

  PlayerInfo* info = playerInfoCreate(name, playerSprite, inventory);

  int atk, def, agi, hp, hpMax, sp, spMax, tp, tpMax, level;
  info->party[0]->atk = _decodeInt(&line, &len, fp, buf);
  info->party[0]->def = _decodeInt(&line, &len, fp, buf);
  info->party[0]->agi = _decodeInt(&line, &len, fp, buf);
  info->party[0]->hp = _decodeInt(&line, &len, fp, buf);
  info->party[0]->hpMax = _decodeInt(&line, &len, fp, buf);
  info->party[0]->sp = _decodeInt(&line, &len, fp, buf);
  info->party[0]->spMax = _decodeInt(&line, &len, fp, buf);
  info->party[0]->tp = _decodeInt(&line, &len, fp, buf);
  info->party[0]->tpMax = _decodeInt(&line, &len, fp, buf);
  info->level = _decodeInt(&line, &len, fp, buf);

  info->xp = _decodeULong(&line, &len, fp, buf);
  info->xpForLevelUp = _decodeULong(&line, &len, fp, buf);
  info->nextRentDate = _decodeULong(&line, &len, fp, buf);
  info->renting = (bool)_decodeInt(&line, &len, fp, buf);
  info->working = (bool)_decodeInt(&line, &len, fp, buf);
  info->alreadyHealed = (bool)_decodeInt(&line, &len, fp, buf);

  info->inventory->weapon = ItemListGetItem(items, _decodeInt(&line, &len, fp, buf));
  info->inventory->offHand = ItemListGetItem(items, _decodeInt(&line, &len, fp, buf));
  info->inventory->head = ItemListGetItem(items, _decodeInt(&line, &len, fp, buf));
  info->inventory->body = ItemListGetItem(items, _decodeInt(&line, &len, fp, buf));
  info->inventory->legs = ItemListGetItem(items, _decodeInt(&line, &len, fp, buf));
  info->inventory->numEquippedItems = _decodeInt(&line, &len, fp, buf);
  info->inventory->numBackpackItems = _decodeInt(&line, &len, fp, buf);
  for (int i = 0; i < info->inventory->numBackpackItems; i++) {
    info->inventory->backpack[i] = ItemListGetItem(items, _decodeInt(&line, &len, fp, buf));
  }
  info->inventory->money = _decodeInt(&line, &len, fp, buf);

  GLOBAL_TIME.val = (unsigned char)_decodeInt(&line, &len, fp, buf);
  GLOBAL_TIME.day = _decodeULong(&line, &len, fp, buf);
  GLOBAL_TIME.phase = (TimeStatePhase)_decodeInt(&line, &len, fp, buf);

  int numTechs = _decodeInt(&line, &len, fp, buf);
  for (int i = 0; i < numTechs; i++) {
    techAddToList(info->party[0]->techList, _decodeInt(&line, &len, fp, buf));
  }
  int numSpecials = _decodeInt(&line, &len, fp, buf);
  for (int i = 0; i < numSpecials; i++) {
    IntListAdd(&info->party[0]->specials, _decodeInt(&line, &len, fp, buf));
  }

  COISpriteSetPos(info->party[0]->sprite, _decodeInt(&line, &len, fp, buf), _decodeInt(&line, &len, fp, buf));


  fclose(fp);

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
