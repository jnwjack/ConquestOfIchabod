#include "player.h"
#include "special.h"

static unsigned int LEVELUP_TECHS_FIGHTER[] = { TECH_ID_FOCUS, TECH_ID_COUNTER, TECH_ID_BRACE, TECH_ID_RAGE };
static unsigned int LEVELUP_SPECIALS_FIGHTER[] = { SPECIAL_ID_PARRY };
static unsigned int LEVELUP_TECHS_MIN_LEVELS_FIGHTER[] = { 1, 5, 7, 11 };
static unsigned int LEVELUP_SPECIALS_MIN_LEVELS_FIGHTER[] = { 1 };
#define LEVELUP_NUM_TECHS_FIGHTER 4
#define LEVELUP_NUM_SPECIALS_FIGHTER 1

static unsigned int LEVELUP_TECHS_MAGE[] = { TECH_ID_FOCUS, TECH_ID_QUICKSTRIKE };
static unsigned int LEVELUP_SPECIALS_MAGE[] = { SPECIAL_ID_ICE_SPEAR, SPECIAL_ID_HEAL, SPECIAL_ID_AVALANCHE, SPECIAL_ID_NEUTRALIZE, SPECIAL_ID_FIREBALL, SPECIAL_ID_CURSE, SPECIAL_ID_SILENCE };
static unsigned int LEVELUP_TECHS_MIN_LEVELS_MAGE[] = { 1, 7 };
static unsigned int LEVELUP_SPECIALS_MIN_LEVELS_MAGE[] = { 1, 3, 5, 5, 7, 9, 9 };
#define LEVELUP_NUM_TECHS_MAGE 2
#define LEVELUP_NUM_SPECIALS_MAGE 7

static unsigned int LEVELUP_TECHS_ROGUE[] = { TECH_ID_FOCUS, TECH_ID_EAGLEEYE, TECH_ID_QUICKSTRIKE };
static unsigned int LEVELUP_SPECIALS_ROGUE[] = { SPECIAL_ID_BACKSTAB, SPECIAL_ID_HASTE, SPECIAL_ID_PARRY, SPECIAL_ID_NEUTRALIZE };
// static unsigned int LEVELUP_TECHS_MIN_LEVELS_ROGUE[] = { 1, 5, 7 };
static unsigned int LEVELUP_TECHS_MIN_LEVELS_ROGUE[] = { 1, 1, 1 };
static unsigned int LEVELUP_SPECIALS_MIN_LEVELS_ROGUE[] = { 1, 3, 5, 8 };
#define LEVELUP_NUM_TECHS_ROGUE 3
#define LEVELUP_NUM_SPECIALS_ROGUE 4

static unsigned int LEVELUP_SPECIALS_CLERK[] = { SPECIAL_ID_BAG_GROCERIES, SPECIAL_ID_CLEAN, SPECIAL_ID_STOCK_SHELVES, SPECIAL_ID_SLACK_OFF, SPECIAL_ID_DAYDREAM, SPECIAL_ID_WATCH_CLOCK };
static unsigned int LEVELUP_SPECIALS_MIN_LEVELS_CLERK[] = { 1, 1, 1, 6, 6, 10 };
#define LEVELUP_NUM_SPECIALS_CLERK 6

SpriteAge playerSpriteAgeFromGlobalTime() {
  if (GLOBAL_TIME.day < 150) {
    return SA_YOUNG;
  } else if (GLOBAL_TIME.day < 250) {
    return SA_OLDER;
  }
  return SA_OLDEST;
}

int playerSpriteIndexFromSpriteAge(SpriteAge age) {
  switch (age) {
  case SA_YOUNG:
    return 1;
  case SA_OLDER:
    return 63;
  default:
    return 64;
  }
}

PlayerInfo* playerInfoCreate(char* name,  COISprite* sprite, Inventory* inventory, int class) {
  PlayerInfo* info = malloc(sizeof(PlayerInfo));

  info->class = class;
  info->inventory = inventory;
  TimeStateCopyGlobalTime(&info->lastXPGain);
  info->party = malloc(sizeof(Actor*) * MAX_PARTY_SIZE);
  info->party[0] = actorCreatePlayer(sprite);
  info->partySize = 1;
  info->level = 1;
  info->xp = 0;
  info->xpForLevelUp = 50;
  info->renting = RS_NOT_RENTING;
  info->spriteAge = SA_YOUNG;
  info->working = false;
  info->alreadyHealed = false;
  info->rentHouseBaldUsed = false;
  info->foundMythicalSword = false;
  info->nextRentDate = 3;
  info->shiftsWorked = 0;
  info->classProgression.specialsIndex = 0;
  info->classProgression.techsIndex = 0;

  // Should change based on class type
  if (info->class == PLAYER_CLASS_WIZARD) {
    info->classProgression.specials = LEVELUP_SPECIALS_MAGE;
    info->classProgression.techs = LEVELUP_TECHS_MAGE;
    info->classProgression.numTechs = LEVELUP_NUM_TECHS_MAGE;
    info->classProgression.numSpecials = LEVELUP_NUM_SPECIALS_MAGE;
    info->classProgression.specialsLevels = LEVELUP_SPECIALS_MIN_LEVELS_MAGE;
    info->classProgression.techsLevels = LEVELUP_TECHS_MIN_LEVELS_MAGE;
  } else if (info->class == PLAYER_CLASS_FIGHTER) {
    info->classProgression.specials = LEVELUP_SPECIALS_FIGHTER;
    info->classProgression.techs = LEVELUP_TECHS_FIGHTER;
    info->classProgression.numTechs = LEVELUP_NUM_TECHS_FIGHTER;
    info->classProgression.numSpecials = LEVELUP_NUM_SPECIALS_FIGHTER;
    info->classProgression.specialsLevels = LEVELUP_SPECIALS_MIN_LEVELS_FIGHTER;
    info->classProgression.techsLevels = LEVELUP_TECHS_MIN_LEVELS_FIGHTER;
  } else {
    info->classProgression.specials = LEVELUP_SPECIALS_ROGUE;
    info->classProgression.techs = LEVELUP_TECHS_ROGUE;
    info->classProgression.numTechs = LEVELUP_NUM_TECHS_ROGUE;
    info->classProgression.numSpecials = LEVELUP_NUM_SPECIALS_ROGUE;
    info->classProgression.specialsLevels = LEVELUP_SPECIALS_MIN_LEVELS_ROGUE;
    info->classProgression.techsLevels = LEVELUP_TECHS_MIN_LEVELS_ROGUE;
  }

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
    // Increase stats
    info->party[0]->atk.base += generateRandomCharInRange(1, 3);
    info->party[0]->def.base += generateRandomCharInRange(1, 3);
    info->party[0]->agi.base += generateRandomCharInRange(1, 3);
    int hpIncrease = generateRandomCharInRange(5, 15);
    info->party[0]->hpMax += hpIncrease;
    info->party[0]->hp += hpIncrease;
    int spIncrease = generateRandomCharInRange(3, 6);
    info->party[0]->spMax += spIncrease;
    info->party[0]->sp += spIncrease;
    int tpIncrease = generateRandomCharInRange(3, 6);
    info->party[0]->tpMax += tpIncrease;
    info->party[0]->tp += tpIncrease;
  }
  TimeStateCopyGlobalTime(&info->lastXPGain);
}

int playerAdjustedATK(PlayerInfo* info) {
  Actor* player = info->party[0];
  Inventory* inventory = info->inventory;
  if (inventory->offHand->type == WEAPON) {
    return ItemAdjustStat(actorModifiedAtk(player), inventory->weapon->strength + inventory->offHand->strength);
  } else {
    return ItemAdjustStat(actorModifiedAtk(player), inventory->weapon->strength);
  }
}

int playerAdjustedDEF(PlayerInfo* info) {
  Actor* player = info->party[0];
  Inventory* inventory = info->inventory;
  int defItemTotal = inventoryDEFItemTotalStrength(inventory);
  return ItemAdjustStat(actorModifiedDef(player), defItemTotal);
}

int playerAdjustedAGI(PlayerInfo* info) {
  Actor* player = info->party[0];
  return actorModifiedAgi(player);
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

  unsigned char normalized = c - '!';
  unsigned char offset = normalized + shift;
  const unsigned char numVals = 'z' - '!';
  return (char)((offset % numVals) + '!');
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
    output[i] = _rotateChar((unsigned char)string[i], 74);
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

void _encodeTimeState(TimeState* ts, char* buf, FILE* fp) {
  _encodeInt((int)ts->val, buf, fp);
  _encodeULong(ts->day, buf, fp);
  _encodeInt((int)ts->phase, buf, fp);
}

void _decodeTimeState(char** line, size_t* len, FILE* fp, char* buf, TimeState* ts) {
  ts->val = (unsigned char)_decodeInt(line, len, fp, buf);
  ts->day = _decodeULong(line, len, fp, buf);
  ts->phase = (TimeStatePhase)_decodeInt(line, len, fp, buf);
}

void _encodeStat(Stat* val, char* buf, FILE* fp) {
  _encodeInt(val->base, buf, fp);
  snprintf(buf, MAX_STRING_SIZE, "%f", val->factor);
  _encodeString(buf, fp);
  _encodeTimeState(&val->end, buf, fp);
}

void _decodeStat(char** line, size_t* len, FILE* fp, char* buf, Stat* stat) {
  stat->base = _decodeInt(line, len, fp, buf);

  getline(line, len, fp);
  _decodeString(*line, buf);
  stat->factor = atof(buf);
  
  _decodeTimeState(line, len, fp, buf, &stat->end);
}

void playerEncode(PlayerInfo* info) {
  FILE* fp = fopen("src/engine/etc/save.dat", "w");

  _encodeString(info->name, fp);

  char temp[MAX_STRING_SIZE];

  _encodeInt(info->class, temp, fp);

  // Stats
  _encodeStat(&info->party[0]->atk, temp, fp);
  _encodeStat(&info->party[0]->def, temp, fp);
  _encodeStat(&info->party[0]->agi, temp, fp);
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
  _encodeTimeState(&GLOBAL_TIME, temp, fp);

  // Abilities
  _encodeInt(info->party[0]->techList->count, temp, fp);
  for (int i = 0; i < info->party[0]->techList->count; i++) {
    Tech* tech = info->party[0]->techList->techs[i];
    _encodeInt(info->party[0]->techList->techs[i]->id, temp, fp);
  }
  _encodeInt(info->party[0]->specials.length, temp, fp);
  printf("num specials: %i\n", info->party[0]->specials.length);
  for (int i = 0; i < info->party[0]->specials.length; i++) {
    _encodeInt(info->party[0]->specials.values[i], temp, fp);
  }

  // Player position
  _encodeInt(info->party[0]->sprite->_x, temp, fp);
  _encodeInt(info->party[0]->sprite->_y, temp, fp);

  _encodeInt((int)info->rentHouseBaldUsed, temp, fp);
  _encodeInt((int)info->shiftsWorked, temp, fp);

  _encodeTimeState(&info->lastXPGain, temp, fp);

  _encodeInt((int)info->foundMythicalSword, temp, fp);


  fclose(fp);
}

bool playerSaveExists() {
  FILE* fp = fopen("src/engine/etc/save.dat", "r");
  if (fp == NULL) {
    return false;
  }
  fclose(fp);
  return true;
}

PlayerInfo* playerDecode(ItemList* items, COISprite* playerSprite, Inventory* inventory) {
  FILE* fp = fopen("src/engine/etc/save.dat", "r");

  char* line = NULL;
  char buf[MAX_STRING_SIZE];
  char name[MAX_STRING_SIZE];
  size_t len = 0;

  getline(&line, &len, fp);
  _decodeString(line, name);
  int class = _decodeInt(&line, &len, fp, buf);

  PlayerInfo* info = playerInfoCreate(name, playerSprite, inventory, class);

  int atk, def, agi, hp, hpMax, sp, spMax, tp, tpMax, level;
  _decodeStat(&line, &len, fp, buf, &info->party[0]->atk);
  _decodeStat(&line, &len, fp, buf, &info->party[0]->def);
  _decodeStat(&line, &len, fp, buf, &info->party[0]->agi);
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

  _decodeTimeState(&line, &len, fp, buf, &GLOBAL_TIME);

  int numTechs = _decodeInt(&line, &len, fp, buf);
  for (int i = 0; i < numTechs; i++) {
    techAddToList(info->party[0]->techList, _decodeInt(&line, &len, fp, buf));
  }
  int numSpecials = _decodeInt(&line, &len, fp, buf);
  for (int i = 0; i < numSpecials; i++) {
    IntListAdd(&info->party[0]->specials, _decodeInt(&line, &len, fp, buf));
  }

  COISpriteSetPos(info->party[0]->sprite, _decodeInt(&line, &len, fp, buf), _decodeInt(&line, &len, fp, buf));

  info->rentHouseBaldUsed = (bool)_decodeInt(&line, &len, fp, buf);
  info->shiftsWorked = (unsigned int)_decodeInt(&line, &len, fp, buf);

  _decodeTimeState(&line, &len, fp, buf, &info->lastXPGain);

  info->foundMythicalSword = (bool)_decodeInt(&line, &len, fp, buf);

  playerUpdateClassProgressionFromTime(info);

  fclose(fp);

  // info->spriteAge = playerSpriteAgeFromGlobalTime();

  return info;
}

char* playerGetClass(PlayerInfo* pInfo) {
  // If we've worked at the shop enough, the class changes.
  if (pInfo->shiftsWorked > 2) {
    return "Clerk";
  }

  return playerClassNameFromID(pInfo->class);
}
 
char* playerClassNameFromID(int id) {
  switch (id) {
  case PLAYER_CLASS_FIGHTER:
    return "Fighter";
  case PLAYER_CLASS_WIZARD:
    return "Mage";
  case PLAYER_CLASS_ROGUE:
    return "Rogue";
  default:
    printf("Error: Invalid class ID\n");
    return "NONE";
  }
}

void playerCheckForEviction(PlayerInfo* pInfo) {
  long daysLeft = (long)pInfo->nextRentDate - (long)GLOBAL_TIME.day;
  if (pInfo->renting == RS_RENTING && daysLeft < 0) {
    pInfo->renting = RS_EVICTED;
  }
}

void playerLevelDown(PlayerInfo* pInfo) {
  if (pInfo->level > 1) {
    pInfo->level--;

    Actor* player = pInfo->party[0];

    // Reduce ability scores
    player->atk.base = MAX(1, player->atk.base - generateRandomCharInRange(1, 3));
    player->def.base = MAX(1, player->def.base - generateRandomCharInRange(1, 3));
    player->agi.base = MAX(1, player->agi.base - generateRandomCharInRange(1, 3));
    player->hpMax = MAX(1, player->hpMax - generateRandomCharInRange(5, 15));
    player->hp = MIN(player->hp, player->hpMax);
    player->spMax = MAX(1, player->spMax - generateRandomCharInRange(3, 6));
    player->sp = MIN(player->sp, player->spMax);
    player->tpMax = MAX(1, player->tpMax - generateRandomCharInRange(3, 6));
    player->tp = MIN(player->tp, player->tpMax);

    // Remove an ability if we can
    int totalAbilities = player->specials.length + player->techList->count;
    if (totalAbilities > 0) {
      int index = generateRandomCharInRange(0, totalAbilities - 1);
      if (index < player->specials.length) {
        // Remove from specials
        IntListDelete(&player->specials, index);
      } else {
        // Remove from techs
        techRemoveFromList(player->techList, player->techList->techs[index - player->specials.length]);
      }
    }

    // Reset timer
    TimeStateCopyGlobalTime(&pInfo->lastXPGain);
  }
}

void playerUpdateClassProgressionFromTime(PlayerInfo* pInfo) {
  if (pInfo->shiftsWorked >= 20) {
    pInfo->classProgression.specials = LEVELUP_SPECIALS_CLERK;
    pInfo->classProgression.numSpecials = LEVELUP_NUM_SPECIALS_CLERK;
    pInfo->classProgression.specialsLevels = LEVELUP_SPECIALS_MIN_LEVELS_CLERK;
  }
}

void playerInfoDestroy(PlayerInfo* info) {
  inventoryDestroy(info->inventory);
  for (int i = 0; i < info->partySize; i++) {
    actorDestroy(info->party[i]);
  }
  free(info->party);
  free(info);
}
