#ifndef BATTLESPLASH_H
#define BATTLESPLASH_H

#include <stdbool.h>
#include "../engine/COIString.h"
#include "../engine/COIBoard.h"
#include "../player.h"

typedef enum {
  BSS_START,
  BSS_RESULT_STRING,
  BSS_REWARDS,
  BSS_PROGRESS_BAR,
  BSS_DONE
} BattleSplashStage;

typedef struct BattleSplash {
  COIString* result;
  COIString* rewards;
  COIString* xp;
  //COIString* low;
  //COIString* high;

  unsigned long _currentProgress;
  unsigned long _currentXP;
  
  COISprite* box;
  BattleSplashStage stage;
  bool animating;
  int _ticks;
} BattleSplash;

BattleSplash* BattleSplashCreate(COIBoard* board,
				 COITextType* textType,
				 COISprite* box,
				 bool victorious,
				 unsigned long currentXP,
				 unsigned long xpForLevelUp,
				 unsigned long gainedXP);
void BattleSplashAnimate(BattleSplash* splash);
bool BattleSplashFinished(BattleSplash* splash);
void BattleSplashDestroy(BattleSplash* splash, COIBoard* board);

#endif
