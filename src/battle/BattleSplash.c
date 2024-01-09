#include "BattleSplash.h"

// Ensure strings are where we want them inside the box
void _positionElements(BattleSplash* splash) {
  COIStringConfineToSprite(splash->result, splash->box);
  COIStringConfineToSprite(splash->rewards, splash->box);
  COIStringConfineToSprite(splash->xp, splash->box);
  COIStringPositionBelowString(splash->rewards, splash->result);
  COIStringPositionBelowString(splash->xp, splash->rewards);
  //COIStringConfineToSprite(splash->high, splash->box);
  //COIStringPositionBelowString(splash->high, splash->rewards);

  COIStringSetVisible(splash->result, false);
  COIStringSetVisible(splash->rewards, false);
  COIStringSetVisible(splash->xp, false);
  //COIStringSetVisible(splash->high, false);
}

BattleSplash* BattleSplashCreate(COIBoard* board,
				 COITextType* textType,
				 COISprite* box,
				 bool victorious,
				 unsigned long currentXP,
				 unsigned long xpForLevelUp,
				 unsigned long gainedXP) {
  BattleSplash* splash = malloc(sizeof(BattleSplash));
  splash->_currentProgress = 0;
  splash->_currentXP = currentXP;
  splash->_ticks = 0;
  splash->box = box;
  splash->box->_autoHandle = false;
  splash->box->_visible = true;
  splash->result = COIStringCreate("Victory", 0, 0, textType);
  COIBoardAddString(board, splash->result);
  // Accept some list of items or something in the future
  splash->rewards = COIStringCreate("Found: Stuff",
				    0,
				    0,
				    textType);
  COIBoardAddString(board, splash->rewards);

  char xpRawString[MAX_STRING_SIZE];
  sprintf(xpRawString, "Gained EXP: %lu", gainedXP); 
  splash->xp = COIStringCreate(xpRawString, 0, 0, textType);
  COIBoardAddString(board, splash->xp);

  /*
  char l[MAX_STRING_SIZE];
  sprintf(l, "%lu", (unsigned long)0);
  splash->low = COIStringCreate(l, 0, 0, textType);
  COIBoardAddString(board, splash->low);

  char h[MAX_STRING_SIZE];
  sprintf(h, "%lu", xpForLevelUp);
  splash->high = COIStringCreate(h, 20, 20, textType);
  COIBoardAddString(board, splash->high);
  */

  // Get ready to start animation
  _positionElements(splash);
  splash->stage = BSS_START;
  return splash;
}

void BattleSplashAnimate(BattleSplash* splash) {
  switch (splash->stage) {
  case BSS_START:
    COIStringSetVisible(splash->result, true);
    splash->stage = BSS_RESULT_STRING;
    break;
  case BSS_RESULT_STRING:
    if (splash->_ticks > 100) {
      splash->_ticks = 0;
      COIStringSetVisible(splash->rewards, true);
      splash->stage = BSS_REWARDS;
    } else {
      splash->_ticks++;
    }
    break;
  case BSS_REWARDS:
    if (splash->_ticks > 100) {
      splash->_ticks = 0;
      COIStringSetVisible(splash->xp, true);
      splash->stage = BSS_PROGRESS_BAR;
    } else {
      splash->_ticks++;
    }
    break;
  case BSS_PROGRESS_BAR:
    if (splash->_ticks > 200) {
      splash->_ticks = 0;
      splash->stage = BSS_DONE;
    } else {
      splash->_ticks++;
    }
    break;
  default:
    splash->_ticks++;
    printf("Invalid BattleSplash Stage.\n");
  }
}

bool BattleSplashFinished(BattleSplash* splash) {
  return splash->stage == BSS_DONE;
}

void BattleSplashDestroy(BattleSplash* splash, COIBoard* board) {
  COIBoardRemoveString(board, splash->result);
  COIStringDestroy(splash->result);
  COIBoardRemoveString(board, splash->rewards);
  COIStringDestroy(splash->rewards);
  COIBoardRemoveString(board, splash->xp);
  COIStringDestroy(splash->xp);
  //COIBoardRemoveString(board, splash->high);
  //COIStringDestroy(splash->high);
  splash->box->_visible = false;
  free(splash);
}
