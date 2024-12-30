#ifndef BATTLESPLASH_H
#define BATTLESPLASH_H

#include <stdbool.h>
#include "../player.h"

#define LEVEL_UP_TECH 0
#define LEVEL_UP_SPECIAL 1
#define LEVEL_UP_STAT 2

typedef enum {
  BSS_START,
  BSS_RESULT_STRING,
  BSS_REWARDS,
  BSS_PROGRESS_BAR,
  BSS_DONE,
  BSS_LEVELUP
} BattleSplashStage;

typedef struct BattleSplash {
  COIString* result;
  COIString* rewards;
  COIString* xp;
  COIString* levelUp;

  LinkedList* additionalStrings;

  unsigned long _currentProgress;
  
  COISprite* box;
  BattleSplashStage stage;
  bool animating;
  int _ticks;
} BattleSplash;

BattleSplash* BattleSplashCreate(COIBoard* board,
				 COITextType* textType,
				 COISprite* box,
				 bool victorious,
				 unsigned long gainedXP,
         bool levelUp,
         unsigned int gold,
         Actor** enemies,
         int numEnemies,
         Inventory* inventory);
void BattleSplashAnimate(BattleSplash* splash, bool cutToEnd);
bool BattleSplashFinished(BattleSplash* splash);
void BattleSplashDestroy(BattleSplash* splash, COIBoard* board);

typedef struct {
  COIString* costStrings[3];
  COIString* descStrings[3];
  COIMenu* menu;
  COISprite* descBox;
  COIMenu* confirmMenu;
  int specialSlots[2];
  int techSlots[2];
} LevelUpSplash;

LevelUpSplash* LevelUpSplashCreate(COIBoard* board, PlayerInfo* pInfo);
bool LevelUpSplashProcessInput(LevelUpSplash* splash, int event);
void LevelUpSplashDestroy(LevelUpSplash* splash, COIBoard* board);
bool LevelUpSplashProcessSelection(LevelUpSplash* splash, PlayerInfo* pInfo);

#endif
