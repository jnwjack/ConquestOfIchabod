#include "BattleSplash.h"
#include "../special.h"

// Ensure strings are where we want them inside the box
void _positionElements(BattleSplash* splash) {
  COIStringConfineToSprite(splash->result, splash->box);
  COIStringConfineToSprite(splash->rewards, splash->box);
  COIStringConfineToSprite(splash->xp, splash->box);
  // COIStringConfineToSprite(splash->levelUp, splash->box);
  COIStringPositionBelowString(splash->rewards, splash->result, true);
  COIStringPositionBelowString(splash->xp, splash->rewards, true);
  // COIStringPositionBelowString(splash->levelUp, splash->xp, false);

  COIStringSetVisible(splash->result, false);
  COIStringSetVisible(splash->rewards, false);
  COIStringSetVisible(splash->xp, false);
  // COIStringSetVisible(splash->levelUp, false);

  if (splash->levelUp) {
    COIStringConfineToSprite(splash->levelUp, splash->box);
    COIStringPositionBelowString(splash->levelUp, splash->xp, false);
    COIStringSetVisible(splash->levelUp, false);
  }
}

BattleSplash* BattleSplashCreate(COIBoard* board,
				 COITextType* textType,
				 COISprite* box,
				 bool victorious,
				 unsigned long gainedXP,
         bool levelUp,
         unsigned int gold) {
  BattleSplash* splash = malloc(sizeof(BattleSplash));
  splash->_currentProgress = 0;
  splash->_ticks = 0;
  splash->box = box;
  splash->box->_autoHandle = false;
  splash->box->_visible = true;

  if (victorious) {
    splash->result = COIStringCreate("Victory", 0, 0, textType);
    COIBoardAddString(board, splash->result);
    // Accept some list of items or something in the future
    char temp[MAX_STRING_SIZE];
    snprintf(temp, MAX_STRING_SIZE, "Found: %u Gold", gold);
    splash->rewards = COIStringCreate(temp,
              0,
              0,
              textType);
    COIBoardAddString(board, splash->rewards);

    sprintf(temp, "Gained EXP: %lu", gainedXP); 
    splash->xp = COIStringCreate(temp, 0, 0, textType);
    COIBoardAddString(board, splash->xp);

    if (levelUp) {
      splash->levelUp = COIStringCreate("Level Up", 0, 0, textType);
      COIBoardAddString(board, splash->levelUp);
    } else {
      splash->levelUp = NULL;
    }
  } else {
    splash->result = COIStringCreate("Defeat", 0, 0, textType);
    COIBoardAddString(board, splash->result);

    splash->rewards = COIStringCreate("Your party has perished.", 0, 0, textType);
    COIBoardAddString(board, splash->rewards);

    splash->xp = COIStringCreate("Your quest is in vain.", 0, 0, textType);
    COIBoardAddString(board, splash->xp);

    splash->levelUp = NULL;
  }

  // Get ready to start animation
  _positionElements(splash);
  splash->stage = BSS_START;
  return splash;
}

void BattleSplashAnimate(BattleSplash* splash, bool cutToEnd) {
  if (cutToEnd && splash->stage < BSS_PROGRESS_BAR) {
    COIStringSetVisible(splash->result, true);
    COIStringSetVisible(splash->rewards, true);
    COIStringSetVisible(splash->xp, true);
    if (splash->levelUp) {
      COIStringSetVisible(splash->levelUp, true);
    }
    splash->stage = BSS_PROGRESS_BAR;
    splash->_ticks = 0;
  }
  
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
      if (splash->levelUp) {
        splash->stage = BSS_LEVELUP;
      } else {
        splash->stage = BSS_PROGRESS_BAR;
      }
    } else {
      splash->_ticks++;
    }
    break;
  case BSS_LEVELUP:
    if (splash->_ticks > 100) {
      splash->_ticks = 0;
      COIStringSetVisible(splash->levelUp, true);
      splash->stage = BSS_PROGRESS_BAR;
    } else {
      splash->_ticks++;
    }
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
  if (splash->levelUp) {
    COIBoardRemoveString(board, splash->levelUp);
    COIStringDestroy(splash->levelUp);
  }
  splash->box->_visible = false;
  free(splash);
}

LevelUpSplash* LevelUpSplashCreate(COIBoard* board, PlayerInfo* pInfo) {
  LevelUpSplash* splash = malloc(sizeof(LevelUpSplash));

  for (int i = 0 ; i < 3; i++) {
    splash->costStrings[i] = NULL;
    splash->descStrings[i] = NULL;
  }

  COITextType* tt = COITextTypeCreate(25, 255, 255, 255, COIWindowGetRenderer(COI_GLOBAL_WINDOW));

  COISprite* menuFrame = COISpriteCreateFromAssetID(50,
                                                  125,
                                                  250,
                                                  125,
                                                  COI_GLOBAL_LOADER,
                                                  5,
                                                  COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COIBoardAddDynamicSprite(board, menuFrame);
  COISprite* menuPointer = COISpriteCreateFromAssetID(0, 0, 32, 32,
                                                  COI_GLOBAL_LOADER,
                                                  6,
                                                  COIWindowGetRenderer(COI_GLOBAL_WINDOW));    
  COIBoardAddDynamicSprite(board, menuPointer);                                            
  splash->menu = COIMenuCreateWithCapacity(menuFrame, menuPointer, 10);


  splash->descBox = COISpriteCreateFromAssetID(325, 125, 275, 200, COI_GLOBAL_LOADER, 5, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  splash->descBox->_autoHandle = false;
  COIBoardAddDynamicSprite(board, splash->descBox);

  // Stat increase
  COIString* increaseName = COIStringCreate("Stat Increase", 0, 0, tt);
  COIStringSetVisible(increaseName, true);
  COIBoardAddString(board, increaseName);
  COIMenuAddString(splash->menu, increaseName, LEVEL_UP_STAT);
  splash->descStrings[2] = COIStringCreate("Additional increase to ATK and DEF", 0, 0, tt);
  splash->costStrings[2] = COIStringCreate("COST: N/A", 0, 0, tt);
  COIStringConfineToSprite(splash->costStrings[2], splash->descBox);
  COIStringConfineToSprite(splash->descStrings[2], splash->descBox);
  COIStringPositionBelowString(splash->descStrings[2], splash->costStrings[2], true);
  COIBoardAddString(board, splash->descStrings[2]);
  COIBoardAddString(board, splash->costStrings[2]);

  // Tech
  if (playerHasValidNextTech(pInfo)) {
    COITextTypeSetColor(tt, 0, 180, 0);
    char temp[MAX_STRING_SIZE];
    Tech* tech = playerGetNextTech(pInfo);
    COIString* techName = techNameAsCOIString(tech, 0, 0, tt, false);
    COIStringSetVisible(techName, true);
    COIBoardAddString(board, techName);
    COIMenuAddString(splash->menu, techName, LEVEL_UP_TECH);
    snprintf(temp, MAX_STRING_SIZE, "COST: %i TP / turn", tech->cost);
    splash->costStrings[0] = COIStringCreate(temp, 0, 0, tt);
    COITextTypeSetColor(tt, 255, 255, 255);
    splash->descStrings[0] = COIStringCreate(techDescFromID(tech->id), 0, 0, tt);
    COIStringSetVisible(splash->descStrings[0], false);
    COIStringSetVisible(splash->costStrings[0], false);
    COIStringConfineToSprite(splash->costStrings[0], splash->descBox);
    COIStringConfineToSprite(splash->descStrings[0], splash->descBox);
    COIStringPositionBelowString(splash->descStrings[0], splash->costStrings[0], true);
    COIBoardAddString(board, splash->descStrings[0]);
    COIBoardAddString(board, splash->costStrings[0]);
  }

  // Special
  if (playerHasValidNextSpecials(pInfo)) {
    COITextTypeSetColor(tt, 5, 246, 250);
    int special = playerGetNextSpecial(pInfo);
    COIString* sName = COIStringCreate(specialName(special), 0, 0, tt);
    COIStringSetVisible(sName, true);
    COIBoardAddString(board, sName);
    COIMenuAddString(splash->menu, sName, LEVEL_UP_SPECIAL);
    char otherTemp[MAX_STRING_SIZE];
    snprintf(otherTemp, MAX_STRING_SIZE, "COST: %i SP", specialCost(special));
    splash->costStrings[1] = COIStringCreate(otherTemp, 0, 0, tt);
    COITextTypeSetColor(tt, 255, 255, 255);
    splash->descStrings[1] = COIStringCreate(specialDescription(special), 0, 0, tt);
    COIStringSetVisible(splash->descStrings[1], false);
    COIStringSetVisible(splash->costStrings[1], false);
    COIStringConfineToSprite(splash->costStrings[1], splash->descBox);
    COIStringConfineToSprite(splash->descStrings[1], splash->descBox);
    COIStringPositionBelowString(splash->descStrings[1], splash->costStrings[1], true);
    COIBoardAddString(board, splash->descStrings[1]);
    COIBoardAddString(board, splash->costStrings[1]);
  }

  // Yes/No menu
  COISprite* confirmFrame = COISpriteCreateFromAssetID(250, 250, 150, 80,
                                                      COI_GLOBAL_LOADER,
                                                      5,
                                                      COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COIBoardAddDynamicSprite(board, confirmFrame);
  COISprite* confirmPointer = COISpriteCreateFromAssetID(0, 0, 32, 32,
                                                        COI_GLOBAL_LOADER,
                                                        6,
                                                        COIWindowGetRenderer(COI_GLOBAL_WINDOW)); 
  COIBoardAddDynamicSprite(board, confirmPointer);  
  splash->confirmMenu = COIMenuCreateWithCapacity(confirmFrame, confirmPointer, 5);
  COIString* no = COIStringCreate("No", 0, 0, tt);
  COIBoardAddString(board, no);
  COIMenuAddString(splash->confirmMenu, no, 0);
  COIString* yes = COIStringCreate("Yes", 0, 0, tt);
  COIBoardAddString(board, yes);
  COIMenuAddString(splash->confirmMenu, yes, 1);
  COIMenuSetInvisible(splash->confirmMenu);

  COIMenuSetVisible(splash->menu);
  splash->descBox->_visible = true;

  return splash;
}

bool LevelUpSplashProcessInput(LevelUpSplash* splash, int event) {
  bool selection;
  if (!splash->confirmMenu->_frame->_visible) {
    bool shouldRefresh = event == MOVING_UP || event == MOVING_DOWN;
      if (shouldRefresh) {
        COIStringSetVisible(splash->descStrings[COIMenuGetCurrentValue(splash->menu)],
                            false);
        COIStringSetVisible(splash->costStrings[COIMenuGetCurrentValue(splash->menu)],
                            false);
      }
      selection = COIMenuHandleInput(splash->menu, event);
      if (shouldRefresh) {
        COIStringSetVisible(splash->descStrings[COIMenuGetCurrentValue(splash->menu)],
                            true);
        COIStringSetVisible(splash->costStrings[COIMenuGetCurrentValue(splash->menu)],
                            true);
      }
  } else {
    selection = COIMenuHandleInput(splash->confirmMenu, event);
  }

  return selection;
}

// Returns true when we're done selecting a bonus
bool LevelUpSplashProcessSelection(LevelUpSplash* splash, PlayerInfo* pInfo) {
  bool done = false;
  if (splash->confirmMenu->_frame->_visible) {
    if (COIMenuGetCurrentValue(splash->confirmMenu) == 1) {
      int selectedUpgrade = COIMenuGetCurrentValue(splash->menu);
      if (selectedUpgrade == LEVEL_UP_TECH) {
        Tech* tech = playerGetNextTech(pInfo);
        techAddToList(pInfo->party[0]->techList, tech->id);
        pInfo->classProgression.techsIndex++;
      } else if (selectedUpgrade == LEVEL_UP_SPECIAL) {
        IntListAdd(&pInfo->party[0]->specials, playerGetNextSpecial(pInfo));
        pInfo->classProgression.specialsIndex++;
      } else {
        pInfo->party[0]->atk += 3;
        pInfo->party[0]->def += 3;
      }
      done = true;
    }
    COIMenuSetInvisible(splash->confirmMenu);
  } else {
    COIMenuSetVisible(splash->confirmMenu);
  }

  return done;
}

void LevelUpSplashDestroy(LevelUpSplash* splash, COIBoard* board) {
  for (int i = 0; i < 3; i++) {

  }

  free(splash);
}
