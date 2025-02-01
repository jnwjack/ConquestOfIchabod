#include "BattleSplash.h"
#include "../special.h"

// Ensure strings are where we want them inside the box
static void _positionElements(BattleSplash* splash) {
  COIStringConfineToSprite(splash->result, splash->box);
  COIStringConfineToSprite(splash->rewards, splash->box);
  COIStringConfineToSprite(splash->xp, splash->box);
  // COIStringConfineToSprite(splash->levelUp, splash->box);
  COIStringPositionBelowString(splash->rewards, splash->result, true);

  COIString* above = splash->rewards;
  LinkedListResetCursor(splash->additionalStrings);
  COIString* current = (COIString*)LinkedListNext(splash->additionalStrings);
  while (current) {
    COIStringConfineToSprite(current, splash->box);
    COIStringPositionBelowString(current, above, true);
    COIStringSetVisible(current, false);
    above = current;
    current = (COIString*)LinkedListNext(splash->additionalStrings);
  }

  COIStringPositionBelowString(splash->xp, above, true);
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

static bool _foundItem(int actorType) {
  switch (actorType) {
  case ACTOR_MEAT_FLAYER:
  case ACTOR_SKELETON:
    return generateRandomBoolWeighted(0.1);
  case ACTOR_WIRE_MAIDEN:
  case ACTOR_WIRE_MOTHER:
    return generateRandomBoolWeighted(0.1);
  case ACTOR_PYROID:
  case ACTOR_VOLCANETTE:
    return generateRandomBoolWeighted(0.1);
  default:
    return false;
  }
}

static int _itemDropFromEnemyType(int actorType) {
  switch (actorType) {
  case ACTOR_MEAT_FLAYER:
  case ACTOR_SKELETON:
  {
    int x = generateRandomCharInRange(0, 6);
    switch (x) {
    case 0:
    case 1:
    case 2:
      return ITEM_ID_KNIFE;
    case 3:
    case 4:
    case 5:
      return ITEM_ID_CRACKED_SHIELD;
    default:
      return ITEM_ID_LEATHER_ARMOR;
    }
  }
  case ACTOR_WIRE_MAIDEN:
  case ACTOR_WIRE_MOTHER:
  {
    int x = generateRandomCharInRange(0, 9);
    switch (x) {
    case 0:
    case 1:
    case 2:
      return ITEM_ID_SP_POTION;
    case 3:
    case 4:
    case 5:
      return ITEM_ID_TP_POTION;
    case 6:
    case 7:
    case 8:
      return ITEM_ID_HEALING_POTION;
    default:
      return ITEM_ID_MOUNTAIN_JUICE;
    }
  }
  case ACTOR_PYROID:
  case ACTOR_VOLCANETTE:
  {
    int x = generateRandomCharInRange(0, 2);
    switch (x) {
    case 0:
      return ITEM_ID_AGI_SCROLL;
    case 1:
      return ITEM_ID_STRENGTH_POTION;
    default:
      return ITEM_ID_DEF_SCROLL;
    }
  }
  default:
    printf("Error: Trying to get item drop from invalid actor.\n");
    return ITEM_ID_TP_POTION;
  }
}

BattleSplash* BattleSplashCreate(COIBoard* board,
				 COITextType* textType,
				 COISprite* box,
				 bool victorious,
				 unsigned long gainedXP,
         bool levelUp,
         unsigned int gold,
         Actor** enemies,
         int numEnemies,
         Inventory* inventory) {
  BattleSplash* splash = malloc(sizeof(BattleSplash));
  splash->_currentProgress = 0;
  splash->_ticks = 0;
  splash->box = box;
  COIBoardAddDynamicSprite(board, splash->box, 0);
  splash->box->_autoHandle = false;
  splash->box->_visible = true;
  splash->additionalStrings = LinkedListCreate();

  if (victorious) {
    splash->result = COIStringCreate("Victory", 0, 0, textType);
    COIBoardAddString(board, splash->result, 0);

    splash->rewards = COIStringCreate("Found:",
              0,
              0,
              textType);
    COIBoardAddString(board, splash->rewards, 0);

    // Gold
    char temp[MAX_STRING_SIZE];
    snprintf(temp, MAX_STRING_SIZE, "%u Gold", gold);
    COIString* goldString = COIStringCreate(temp,
              0,
              0,
              textType);
    COIBoardAddString(board, goldString, 0);
    LinkedListAdd(splash->additionalStrings, (void*)goldString);

    // Did we find items?
    int MAX_ITEMS = 3;
    int foundItems = 0;
    for (int i = 0; i < numEnemies && foundItems < MAX_ITEMS; i++) {
      if (_foundItem(enemies[i]->actorType)) {
        int itemID = _itemDropFromEnemyType(enemies[i]->actorType);
        COIString* string = COIStringCreate(ItemListStringFromItemID(itemID), 0, 0, textType);
        COIBoardAddString(board, string, 0);
        LinkedListAdd(splash->additionalStrings, (void*)string);
        inventoryAddItem(inventory, itemID);
        foundItems++;
      }
    }

    sprintf(temp, "Gained EXP: %lu", gainedXP); 
    splash->xp = COIStringCreate(temp, 0, 0, textType);
    COIBoardAddString(board, splash->xp, 0);

    if (levelUp) {
      splash->levelUp = COIStringCreate("Level Up", 0, 0, textType);
      COIBoardAddString(board, splash->levelUp, 0);
    } else {
      splash->levelUp = NULL;
    }
  } else {
    splash->result = COIStringCreate("Defeat", 0, 0, textType);
    COIBoardAddString(board, splash->result, 0);

    splash->rewards = COIStringCreate("Your party has perished.", 0, 0, textType);
    COIBoardAddString(board, splash->rewards, 0);

    splash->xp = COIStringCreate("Your quest is in vain.", 0, 0, textType);
    COIBoardAddString(board, splash->xp, 0);

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
    LinkedListResetCursor(splash->additionalStrings);
    COIString* additionalString = (COIString*)LinkedListNext(splash->additionalStrings);
    while (additionalString) {
      COIStringSetVisible(additionalString, true);
      additionalString = (COIString*)LinkedListNext(splash->additionalStrings);
    }
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
      LinkedListResetCursor(splash->additionalStrings);
    } else {
      splash->_ticks++;
    }
    break;
  case BSS_REWARDS:
  {
    if (splash->_ticks > 100) {
      splash->_ticks = 0;
      COIStringSetVisible(splash->xp, true);

      COIString* additionalString = (COIString*)LinkedListNext(splash->additionalStrings);      
      if (additionalString) {
        COIStringSetVisible(additionalString, true);
      } else {
        if (splash->levelUp) {
          splash->stage = BSS_LEVELUP;
        } else {
          splash->stage = BSS_PROGRESS_BAR;
        }
      }
    } else {
      splash->_ticks++;
    }
    break;
  }
  case BSS_LEVELUP:
    if (splash->_ticks > 100) {
      splash->_ticks = 0;
      COIStringSetVisible(splash->levelUp, true);
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
  COIBoardRemoveString(board, splash->result, 0);
  COIStringDestroy(splash->result);
  COIBoardRemoveString(board, splash->rewards, 0);
  COIStringDestroy(splash->rewards);
  COIBoardRemoveString(board, splash->xp, 0);
  COIStringDestroy(splash->xp);
  LinkedListResetCursor(splash->additionalStrings);
  COIString* current = (COIString*)LinkedListNext(splash->additionalStrings);
  while (current) {
    COIBoardRemoveString(board, current, 0);
    COIStringDestroy(current);
    current = (COIString*)LinkedListNext(splash->additionalStrings);
  }
  LinkedListDestroy(splash->additionalStrings);
  if (splash->levelUp) {
    COIBoardRemoveString(board, splash->levelUp, 0);
    COIStringDestroy(splash->levelUp);
  }
  COIBoardRemoveDynamicSprite(board, splash->box, 0);
  COISpriteDestroy(splash->box);
  // splash->box->_visible = false;
  free(splash);
}

// Randomly pick 2 abilities for level-up bonus from the pool of available abilities.
static void _getAbilityIndices(int* specialIndices, int* techIndices, PlayerInfo* pInfo) {
  specialIndices[0] = -1;
  specialIndices[1] = -1;
  techIndices[0] = -1;
  techIndices[1] = -1;
  Actor* player = pInfo->party[0];

  // Get possible specials
  IntList sl;
  IntListInitialize(&sl, pInfo->classProgression.numSpecials);
  for (int i = 0; i < sl.capacity; i++) {
    if (pInfo->classProgression.specialsLevels[i] <= pInfo->level) {
      // Don't add it if we already have it.
      bool shouldAdd = true;
      for (int j = 0; j < player->specials.length; j++) {
        if (pInfo->classProgression.specials[i] == player->specials.values[j]) {
          shouldAdd = false;
          break;
        }
      }
      if (shouldAdd) {
        printf("possible special: %i\n", pInfo->classProgression.specials[i]);
        IntListAdd(&sl, pInfo->classProgression.specials[i]);
      }
    }
  }

  // Get possible techs
  IntList tl;
  IntListInitialize(&tl, pInfo->classProgression.numTechs);
  for (int i = 0; i < tl.capacity; i++) {
    if (pInfo->classProgression.techsLevels[i] <= pInfo->level) {
      bool shouldAdd = true;
      for (int j = 0; j < player->techList->count; j++) {
        Tech* currentTech = player->techList->techs[j];
        if (pInfo->classProgression.techs[i] == currentTech->id) {
          shouldAdd = false;
          break;
        }
      }
      if (shouldAdd) {
        printf("possible tech: %i\n", pInfo->classProgression.techs[i]);
        IntListAdd(&tl, pInfo->classProgression.techs[i]);
      }
    }
  }

  int totalCount = sl.length + tl.length;
  if (totalCount < 1) {
    return;
  }

  // Pick the first ability.
  int specialCount = 0;
  int techCount = 0;
  int firstIndex = generateRandomCharInRange(0, totalCount - 1);
  printf("first index: %i\n", firstIndex);
  if (firstIndex < sl.length) {
    specialIndices[specialCount] = sl.values[firstIndex];
    specialCount++;
  } else {
    techIndices[techCount] = tl.values[firstIndex - sl.length];
    techCount++;
  }

  // Pick the second.
  if (totalCount < 2) {
    return;
  }
  int secondIndex = generateRandomCharInRange(0, totalCount - 2);
  int trueIndex; // Skipping over first index if we need to.
  if (secondIndex < firstIndex) {
    trueIndex = secondIndex;
  } else {
    trueIndex = firstIndex + 1 + (secondIndex - firstIndex); // Account for "hole" caused by first index
  }
  printf("secondIndex: %i\n trueIndex: %i\n", secondIndex, trueIndex);
  if (trueIndex < sl.length) {
    specialIndices[specialCount] = sl.values[trueIndex];
    specialCount++;
  } else {
    techIndices[techCount] = tl.values[trueIndex - sl.length];
    techCount++;
  }

  IntListDestroy(&sl);
  IntListDestroy(&tl);
}

static void _applyStatIncrease(PlayerInfo* pInfo) {
  Actor* actor = pInfo->party[0];
  switch (pInfo->class) {
  case PLAYER_CLASS_FIGHTER:
    actor->atk.base += playerGetRandomStatIncrease();
    actor->def.base += playerGetRandomStatIncrease();
    break;
  case PLAYER_CLASS_WIZARD:
  {
    int tpGain = playerGetRandomSPTPIncrease();
    int spGain = playerGetRandomSPTPIncrease();
    actor->tpMax += tpGain;
    actor->tp = MIN(actor->tpMax, actor->tp + tpGain);
    actor->spMax += spGain;
    actor->sp = MIN(actor->spMax, actor->sp + spGain);
    break;
  }
  case PLAYER_CLASS_ROGUE:
  {
    int tpGain = playerGetRandomSPTPIncrease();
    actor->tpMax += tpGain;
    actor->tp = MIN(actor->tpMax, actor->tp + tpGain);
    actor->agi.base += playerGetRandomStatIncrease();
    break;
  }
  default:
    printf("Error: invalid class during level up.\n");
  }
}

static char* _getStatIncreaseString(PlayerInfo* pInfo) {
  switch (pInfo->class) {
  case PLAYER_CLASS_FIGHTER:
    return "Additional increase to ATK and DEF";
  case PLAYER_CLASS_WIZARD:
    return "Additional increase to TP and SP";
  case PLAYER_CLASS_ROGUE:
    return "Additional increase to AGI and TP";
  default:
    printf("Error: invalid class during level up.\n");
    return "ERROR";
  }
}

LevelUpSplash* LevelUpSplashCreate(COIBoard* board, PlayerInfo* pInfo) {
  LevelUpSplash* splash = malloc(sizeof(LevelUpSplash));

  for (int i = 0 ; i < 3; i++) {
    splash->costStrings[i] = NULL;
    splash->descStrings[i] = NULL;
  }

  COITextType* tt = COITextTypeCreate(16, 255, 255, 255, COIWindowGetRenderer(COI_GLOBAL_WINDOW));

  COISprite* menuFrame = COISpriteCreateFromAssetID(25,
                                                  125,
                                                  275,
                                                  125,
                                                  COI_GLOBAL_LOADER,
                                                  5,
                                                  COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COIBoardAddDynamicSprite(board, menuFrame, 0);
  COISprite* menuPointer = COISpriteCreateFromAssetID(0, 0, 32, 32,
                                                  COI_GLOBAL_LOADER,
                                                  6,
                                                  COIWindowGetRenderer(COI_GLOBAL_WINDOW));    
  COIBoardAddDynamicSprite(board, menuPointer, 0);                                            
  splash->menu = COIMenuCreateWithCapacity(menuFrame, menuPointer, 10);


  splash->descBox = COISpriteCreateFromAssetID(325, 125, 275, 200, COI_GLOBAL_LOADER, 5, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  splash->descBox->_autoHandle = false;
  COIBoardAddDynamicSprite(board, splash->descBox, 0);

  // Stat increase
  COIString* increaseName = COIStringCreate("Stat Increase", 0, 0, tt);
  COIStringSetVisible(increaseName, true);
  COIBoardAddString(board, increaseName, 0);
  COIMenuAddString(splash->menu, increaseName, LEVEL_UP_STAT);
  splash->descStrings[0] = COIStringCreate(_getStatIncreaseString(pInfo), 0, 0, tt);
  splash->costStrings[0] = COIStringCreate("COST: N/A", 0, 0, tt);
  COIStringConfineToSprite(splash->costStrings[0], splash->descBox);
  COIStringConfineToSprite(splash->descStrings[0], splash->descBox);
  COIStringPositionBelowString(splash->descStrings[0], splash->costStrings[0], true);
  COIBoardAddString(board, splash->descStrings[0], 0);
  COIBoardAddString(board, splash->costStrings[0], 0);

  int specials[2] = { -1, -1 };
  int techs[2] = { -1, -1 };
  splash->techSlots[0] = -1;
  splash->techSlots[1] = -1;
  splash->specialSlots[0] = -1;
  splash->specialSlots[1] = -1;
  _getAbilityIndices(specials, techs, pInfo);

  char temp[MAX_STRING_SIZE];
  int indexInMenu = 1;
  for (int i = 0; i < 2; i++) {
    if (techs[i] != -1) {
      COITextTypeSetColor(tt, 0, 180, 0);
      splash->techSlots[indexInMenu - 1] = techs[i];
      printf("techs[%i]: %i\n", indexInMenu, techs[i]);
      Tech* tech = techCreate(techs[i]);
      COIString* techName = techNameAsCOIString(tech, 0, 0, tt);
      COIStringSetVisible(techName, true);
      COIBoardAddString(board, techName, 0);
      COIMenuAddString(splash->menu, techName, LEVEL_UP_TECH);
      snprintf(temp, MAX_STRING_SIZE, "COST: %i TP / turn", tech->cost);
      splash->costStrings[indexInMenu] = COIStringCreate(temp, 0, 0, tt);
      COITextTypeSetColor(tt, 255, 255, 255);
      splash->descStrings[indexInMenu] = COIStringCreate(techDescFromID(tech->id), 0, 0, tt);
      COIStringSetVisible(splash->descStrings[indexInMenu], false);
      COIStringSetVisible(splash->costStrings[indexInMenu], false);
      COIStringConfineToSprite(splash->costStrings[indexInMenu], splash->descBox);
      COIStringConfineToSprite(splash->descStrings[indexInMenu], splash->descBox);
      COIStringPositionBelowString(splash->descStrings[indexInMenu], splash->costStrings[indexInMenu], true);
      COIBoardAddString(board, splash->descStrings[indexInMenu], 0);
      COIBoardAddString(board, splash->costStrings[indexInMenu], 0);
      indexInMenu++;
      techDestroy(tech);
    }
  }
  
  for (int i = 0; i < 2 && indexInMenu < 3; i++) {
    if (specials[i] != -1) {
      COITextTypeSetColor(tt, 5, 246, 250);
      splash->specialSlots[indexInMenu - 1] = specials[i];
      printf("specials[%i]: %i\n", indexInMenu, specials[i]);
      COIString* sName = COIStringCreate(specialName(specials[i]), 0, 0, tt);
      COIStringSetVisible(sName, true);
      COIBoardAddString(board, sName, 0);
      COIMenuAddString(splash->menu, sName, LEVEL_UP_SPECIAL);
      snprintf(temp, MAX_STRING_SIZE, "COST: %i SP", specialCost(specials[i]));
      splash->costStrings[indexInMenu] = COIStringCreate(temp, 0, 0, tt);
      COITextTypeSetColor(tt, 255, 255, 255);
      splash->descStrings[indexInMenu] = COIStringCreate(specialDescription(specials[i]), 0, 0, tt);
      COIStringSetVisible(splash->descStrings[indexInMenu], false);
      COIStringSetVisible(splash->costStrings[indexInMenu], false);
      COIStringConfineToSprite(splash->costStrings[indexInMenu], splash->descBox);
      COIStringConfineToSprite(splash->descStrings[indexInMenu], splash->descBox);
      COIStringPositionBelowString(splash->descStrings[indexInMenu], splash->costStrings[indexInMenu], true);
      COIBoardAddString(board, splash->descStrings[indexInMenu], 0);
      COIBoardAddString(board, splash->costStrings[indexInMenu], 0);
      indexInMenu++;
    }
  }


  // Yes/No menu
  COITextTypeSetColor(tt, 255, 255, 255);
  COISprite* confirmFrame = COISpriteCreateFromAssetID(25, 250, 100, 70,
                                                      COI_GLOBAL_LOADER,
                                                      5,
                                                      COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COIBoardAddDynamicSprite(board, confirmFrame, 0);
  COISprite* confirmPointer = COISpriteCreateFromAssetID(0, 0, 32, 32,
                                                        COI_GLOBAL_LOADER,
                                                        6,
                                                        COIWindowGetRenderer(COI_GLOBAL_WINDOW)); 
  COIBoardAddDynamicSprite(board, confirmPointer, 0);  
  splash->confirmMenu = COIMenuCreateWithCapacity(confirmFrame, confirmPointer, 5);
  COIString* no = COIStringCreate("No", 0, 0, tt);
  COIBoardAddString(board, no, 0);
  COIMenuAddString(splash->confirmMenu, no, 0);
  COIString* yes = COIStringCreate("Yes", 0, 0, tt);
  COIBoardAddString(board, yes, 0);
  COIMenuAddString(splash->confirmMenu, yes, 1);
  COIMenuSetInvisible(splash->confirmMenu);

  COIMenuSetVisible(splash->menu);
  splash->descBox->_visible = true;

  COITextTypeDestroy(tt);

  return splash;
}

bool LevelUpSplashProcessInput(LevelUpSplash* splash, int event) {
  bool selection;
  if (!splash->confirmMenu->_frame->_visible) {
    bool shouldRefresh = event == MOVING_UP || event == MOVING_DOWN;
      if (shouldRefresh) {
        // COIStringSetVisible(splash->descStrings[COIMenuGetCurrentValue(splash->menu)],
        //                     false);
        COIStringSetVisible(splash->descStrings[splash->menu->_current],
                            false);
        COIStringSetVisible(splash->costStrings[splash->menu->_current],
                            false);
      }
      selection = COIMenuHandleInput(splash->menu, event, true);
      if (shouldRefresh) {
        COIStringSetVisible(splash->descStrings[splash->menu->_current],
                            true);
        COIStringSetVisible(splash->costStrings[splash->menu->_current],
                            true);
      }
  } else {
    selection = COIMenuHandleInput(splash->confirmMenu, event, true);
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
        int techID = splash->techSlots[splash->menu->_current - 1];
        printf("TECH: %i\n", techID);
        Tech* tech = techCreate(techID);
        // Tech* tech = playerGetNextTech(pInfo);
        techAddToList(pInfo->party[0]->techList, tech->id);
        pInfo->classProgression.techsIndex++;
      } else if (selectedUpgrade == LEVEL_UP_SPECIAL) {
        int specialID = splash->specialSlots[splash->menu->_current - 1];
        printf("SPECIAL: %i\n", specialID);
        IntListAdd(&pInfo->party[0]->specials, specialID);
        pInfo->classProgression.specialsIndex++;
      } else {
        _applyStatIncrease(pInfo);
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
    if (splash->costStrings[i]) {
      COIBoardRemoveString(board, splash->costStrings[i], 0);
      COIStringDestroy(splash->costStrings[i]);
    }
    if (splash->descStrings[i]) {
      COIBoardRemoveString(board, splash->descStrings[i], 0);
      COIStringDestroy(splash->descStrings[i]);
    }
  }

  COIMenuDestroyAndFreeComponents(splash->menu, board);
  COIMenuDestroyAndFreeComponents(splash->confirmMenu, board);

  COIBoardRemoveDynamicSprite(board, splash->descBox, 0);
  COISpriteDestroy(splash->descBox);

  free(splash);
}
