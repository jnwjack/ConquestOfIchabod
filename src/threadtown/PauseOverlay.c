#include "PauseOverlay.h"
#include "../special.h"

static void _removeStringIfExists(PauseOverlay* overlay, COIString* string) {
  if (string != NULL) {
    COIBoardRemoveString(overlay->board, string, 0);
    COIStringDestroy(string);
  }
}

static void _setStringVisibleIfExists(COIString* string, bool visible) {
  if (string != NULL) {
    COIStringSetVisible(string, visible);
  }
}

static void _destroySubMenus(PauseOverlay* overlay) {
  COIMenuDestroyAndFreeComponents(overlay->itemsMenu, overlay->board);
  COIMenuDestroyAndFreeComponents(overlay->weaponsMenu, overlay->board);
  COIMenuDestroyAndFreeComponents(overlay->armorMenu, overlay->board);
}

static void _updateStatChanges(PauseOverlay* overlay, int menuValue) {
  // We only want to show a stat change if the item isn't equipped.
  if (menuValue >= 0) {
    Inventory* inventory = overlay->pInfo->inventory;
    Item* item = inventory->backpack[menuValue];
    char temp[MAX_STRING_SIZE];
    
    // Temporarily 'equip' the item so we can see how it would affect stats.
    int oldHP = playerAdjustedHP(overlay->pInfo);
    int oldSP = playerAdjustedSP(overlay->pInfo);
    int oldTP = playerAdjustedTP(overlay->pInfo);
    int oldATK = playerAdjustedATK(overlay->pInfo);
    int oldDEF = playerAdjustedDEF(overlay->pInfo);
    int oldAGI = playerAdjustedAGI(overlay->pInfo);
    Item* oldItem = inventoryEquipItem(inventory, item);
    int newHP = playerAdjustedHP(overlay->pInfo);
    int newSP = playerAdjustedSP(overlay->pInfo);
    int newTP = playerAdjustedTP(overlay->pInfo);
    int newATK = playerAdjustedATK(overlay->pInfo);
    int newDEF = playerAdjustedDEF(overlay->pInfo);
    int newAGI = playerAdjustedAGI(overlay->pInfo);    
    inventoryEquipItem(inventory, oldItem);

    printf("OLD: %i NEW: %i\n", oldATK, newATK);

    // We should only display a stat change string if there's an actual stat change.
    if (newHP != oldHP) {
      _removeStringIfExists(overlay, overlay->hpChange);
      snprintf(temp, MAX_STRING_SIZE, "%i", newHP);
      overlay->hpChange = COIStringCreate(temp, 0, 0,
					   newHP > oldHP ? overlay->posTextType : overlay->negTextType);
      COIStringConfineToSprite(overlay->hpChange, overlay->statWindow);
      COIStringPositionBelowString(overlay->hpChange, overlay->class, false);
      COIStringPositionRightOfString(overlay->hpChange, overlay->hp, 10);
      COIBoardAddString(overlay->board, overlay->hpChange, 0);
      _setStringVisibleIfExists(overlay->hpChange, true);      
    } else {
      _setStringVisibleIfExists(overlay->hpChange, false);
    }

    if (newSP != oldSP) {
      _removeStringIfExists(overlay, overlay->spChange);
      snprintf(temp, MAX_STRING_SIZE, "%i", newSP);
      overlay->spChange = COIStringCreate(temp, 0, 0,
					   newSP > oldSP ? overlay->posTextType : overlay->negTextType);
      COIStringConfineToSprite(overlay->spChange, overlay->statWindow);
      COIStringPositionBelowString(overlay->spChange, overlay->hp, false);
      COIStringPositionRightOfString(overlay->spChange, overlay->sp, 10);
      COIBoardAddString(overlay->board, overlay->spChange, 0);
      _setStringVisibleIfExists(overlay->spChange, true);      
    } else {
      _setStringVisibleIfExists(overlay->spChange, false);
    }

    if (newTP != oldTP) {
      _removeStringIfExists(overlay, overlay->tpChange);
      snprintf(temp, MAX_STRING_SIZE, "%i", newTP);
      overlay->tpChange = COIStringCreate(temp, 0, 0,
					   newTP > oldTP ? overlay->posTextType : overlay->negTextType);
      COIStringConfineToSprite(overlay->tpChange, overlay->statWindow);
      COIStringPositionBelowString(overlay->tpChange, overlay->sp, false);
      COIStringPositionRightOfString(overlay->tpChange, overlay->tp, 10);
      COIBoardAddString(overlay->board, overlay->tpChange, 0);
      _setStringVisibleIfExists(overlay->tpChange, true);      
    } else {
      _setStringVisibleIfExists(overlay->tpChange, false);
    }

    if (newATK != oldATK) {
      _removeStringIfExists(overlay, overlay->atkChange);
      snprintf(temp, MAX_STRING_SIZE, "%i", newATK);
      overlay->atkChange = COIStringCreate(temp, 0, 0,
					   newATK > oldATK ? overlay->posTextType : overlay->negTextType);
      COIStringConfineToSprite(overlay->atkChange, overlay->statWindow);
      COIStringPositionBelowString(overlay->atkChange, overlay->tp, false);
      COIStringPositionRightOfString(overlay->atkChange, overlay->atk, 10);
      COIBoardAddString(overlay->board, overlay->atkChange, 0);
      _setStringVisibleIfExists(overlay->atkChange, true);      
    } else {
      _setStringVisibleIfExists(overlay->atkChange, false);
    }

    if (newDEF != oldDEF) {
      _removeStringIfExists(overlay, overlay->defChange);
      snprintf(temp, MAX_STRING_SIZE, "%i", newDEF);
      overlay->defChange = COIStringCreate(temp, 0, 0,
					   newDEF > oldDEF ? overlay->posTextType : overlay->negTextType);
      COIStringConfineToSprite(overlay->defChange, overlay->statWindow);
      COIStringPositionBelowString(overlay->defChange, overlay->atk, false);
      COIStringPositionRightOfString(overlay->defChange, overlay->def, 10);
      COIBoardAddString(overlay->board, overlay->defChange, 0);
      _setStringVisibleIfExists(overlay->defChange, true);
    } else {
      _setStringVisibleIfExists(overlay->defChange, false);
    }

    if (newAGI != oldAGI) {
      _removeStringIfExists(overlay, overlay->agiChange);
      snprintf(temp, MAX_STRING_SIZE, "%i", newAGI);
      overlay->agiChange = COIStringCreate(temp, 0, 0,
					   newAGI > oldAGI ? overlay->posTextType : overlay->negTextType);
      COIStringConfineToSprite(overlay->agiChange, overlay->statWindow);
      COIStringPositionBelowString(overlay->agiChange, overlay->def, false);
      COIStringPositionRightOfString(overlay->agiChange, overlay->agi, 10);
      COIBoardAddString(overlay->board, overlay->defChange, 0);
      _setStringVisibleIfExists(overlay->agiChange, true);
    } else {
      _setStringVisibleIfExists(overlay->agiChange, false);
    }

  } else {
    // Item is equipped. No visible change strings.
    _setStringVisibleIfExists(overlay->hpChange, false);
    _setStringVisibleIfExists(overlay->spChange, false);
    _setStringVisibleIfExists(overlay->tpChange, false);
    _setStringVisibleIfExists(overlay->atkChange, false);
    _setStringVisibleIfExists(overlay->defChange, false);
    _setStringVisibleIfExists(overlay->agiChange, false);
  }
}

// Return green, red, or regular text type depending on if we
// have an adjusted stat.
COITextType* _textTypeForStat(PauseOverlay* overlay, Stat* stat) {
  COITextType* textType = overlay->textType;
  int modifiedStat = actorModifiedStat(stat);
  if (modifiedStat != stat->base) {
    textType = modifiedStat < stat->base ? overlay->negTextType : overlay->posTextType;
  }

  return textType;
}

static void _makeGoldLabelAndString(PauseOverlay* overlay) {
  _removeStringIfExists(overlay, overlay->goldLabel);
  _removeStringIfExists(overlay, overlay->gold);

  char temp[MAX_STRING_SIZE];
  if (GLOBAL_TIME.day >= PAUSE_OVERLAY_GOLD_DAYS) {
    overlay->goldLabel = COIStringCreate("Days:", 0, 0, overlay->textType);
    snprintf(temp, MAX_STRING_SIZE, "%lu", 300 - GLOBAL_TIME.day);
    overlay->gold = COIStringCreate(temp, 0, 0, overlay->textType);
  } else {
    overlay->goldLabel = COIStringCreate("Gold:", 0, 0, overlay->textType);
    snprintf(temp, MAX_STRING_SIZE, "%i", overlay->pInfo->inventory->money);
    overlay->gold = COIStringCreate(temp, 0, 0, overlay->textType);
  }
  COIStringConfineToSprite(overlay->goldLabel, overlay->statWindow);
  COIStringPositionBelowString(overlay->goldLabel, overlay->agiLabel, false);
  COIBoardAddString(overlay->board, overlay->goldLabel, 0);
  COIStringPositionBelowString(overlay->gold, overlay->agi, false);
  COIBoardAddString(overlay->board, overlay->gold, 0);
}

static void _makeStatStrings(PauseOverlay* overlay) {
  Actor* player = overlay->pInfo->party[0];
  char temp[MAX_STRING_SIZE];
  
  snprintf(temp, MAX_STRING_SIZE, "%i/%i", player->hp, player->hpMax);  
  overlay->hp = COIStringCreate(temp, 0, 0, overlay->textType);
  COIStringPositionBelowString(overlay->hp, overlay->lvLabel, false);
  // COIStringPositionRightOfString(overlay->hp, overlay->hpLabel, 50);
  COIBoardAddString(overlay->board, overlay->hp, 0);

  snprintf(temp, MAX_STRING_SIZE, "%i/%i", player->sp, player->spMax);
  overlay->sp = COIStringCreate(temp, 0, 0, overlay->textType);
  COIStringPositionBelowString(overlay->sp, overlay->hp, false);
  COIBoardAddString(overlay->board, overlay->sp, 0);

  snprintf(temp, MAX_STRING_SIZE, "%i/%i", player->tp, player->tpMax);
  overlay->tp = COIStringCreate(temp, 0, 0, overlay->textType);
  COIStringPositionBelowString(overlay->tp, overlay->sp, false);
  COIBoardAddString(overlay->board, overlay->tp, 0);

  snprintf(temp, MAX_STRING_SIZE, "%i", playerAdjustedATK(overlay->pInfo));
  overlay->atk = COIStringCreate(temp, 0, 0, _textTypeForStat(overlay, &player->atk));
  COIStringPositionBelowString(overlay->atk, overlay->tp, false);
  COIBoardAddString(overlay->board, overlay->atk, 0);

  snprintf(temp, MAX_STRING_SIZE, "%i", playerAdjustedDEF(overlay->pInfo));
  overlay->def = COIStringCreate(temp, 0, 0, _textTypeForStat(overlay, &player->def));
  COIStringPositionBelowString(overlay->def, overlay->atk, false);
  COIBoardAddString(overlay->board, overlay->def, 0);

  snprintf(temp, MAX_STRING_SIZE, "%i", playerAdjustedAGI(overlay->pInfo));
  overlay->agi = COIStringCreate(temp, 0, 0, _textTypeForStat(overlay, &player->agi));
  COIStringPositionBelowString(overlay->agi, overlay->def, false);
  COIBoardAddString(overlay->board, overlay->agi, 0);

  // snprintf(temp, MAX_STRING_SIZE, "%i", overlay->pInfo->inventory->money);
  // overlay->gold = COIStringCreate(temp, 0, 0, overlay->textType);
  // COIStringPositionBelowString(overlay->gold, overlay->agi, false);
  // COIBoardAddString(overlay->board, overlay->gold);
  _makeGoldLabelAndString(overlay);
}

static void _makeLevelString(PauseOverlay* overlay) {
  char temp[MAX_STRING_SIZE];

  snprintf(temp, MAX_STRING_SIZE, "%u", overlay->pInfo->level);
  overlay->lv = COIStringCreate(temp, 0, 0, overlay->textType);
  COIBoardAddString(overlay->board, overlay->lv, 0);
  COIStringPositionBelowString(overlay->lv, overlay->name, true);
  COIStringPositionRightOfString(overlay->lv, overlay->lvLabel, PAUSE_OVERLAY_LABEL_STAT_PADDING);
}

static void _makeClassString(PauseOverlay* overlay) {
  overlay->class = COIStringCreate(playerGetClass(overlay->pInfo), 0, 0, overlay->textType);
  COIStringConfineToSprite(overlay->class, overlay->statWindow);
  COIStringPositionBelowString(overlay->class, overlay->name, false);
  COIBoardAddString(overlay->board, overlay->class, 0);
}

static void _makeStatWindow(PauseOverlay* overlay, PlayerInfo* pInfo, COITextType* textType, COIBoard* board) {
  Actor* player = pInfo->party[0];
  
  overlay->statWindow = COISpriteCreateFromAssetID(70, 40, 250, 250,
						   COI_GLOBAL_LOADER,
						   5,
						   COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  overlay->statWindow->_autoHandle = false;
  COIBoardAddDynamicSprite(board, overlay->statWindow, 0);
  overlay->name = COIStringCreate(pInfo->name, 0, 0, textType);
  // overlay->class = COIStringCreate(playerGetClass(pInfo), 0, 0, textType);
  COIStringConfineToSprite(overlay->name, overlay->statWindow);
  COIBoardAddString(board, overlay->name, 0);
  _makeClassString(overlay);
  // COIStringConfineToSprite(overlay->class, overlay->statWindow);
  // COIStringPositionBelowString(overlay->class, overlay->name, false);
  // COIBoardAddString(board, overlay->class);
  overlay->lvLabel = COIStringCreate("Lv:", 0, 0, textType);
  COIBoardAddString(board, overlay->lvLabel, 0);
  COIStringPositionBelowString(overlay->lvLabel, overlay->name, true);
  COIStringSetPos(overlay->lvLabel, overlay->statWindow->_x + 130, overlay->lvLabel->y);
  _makeLevelString(overlay);
  
  overlay->hpLabel = COIStringCreate("HP:", 0, 0, textType);
  COIStringConfineToSprite(overlay->hpLabel, overlay->statWindow);
  COIStringPositionBelowString(overlay->hpLabel, overlay->class, false);
  COIBoardAddString(board, overlay->hpLabel, 0);

  overlay->spLabel = COIStringCreate("SP:", 0, 0, textType);
  COIStringConfineToSprite(overlay->spLabel, overlay->statWindow);
  COIStringPositionBelowString(overlay->spLabel, overlay->hpLabel, false);
  COIBoardAddString(board, overlay->spLabel, 0);

  overlay->tpLabel = COIStringCreate("TP:", 0, 0, textType);
  COIStringConfineToSprite(overlay->tpLabel, overlay->statWindow);
  COIStringPositionBelowString(overlay->tpLabel, overlay->spLabel, false);
  COIBoardAddString(board, overlay->tpLabel, 0);
  
  overlay->atkLabel = COIStringCreate("ATK:", 0, 0, textType);
  COIStringConfineToSprite(overlay->atkLabel, overlay->statWindow);
  COIStringPositionBelowString(overlay->atkLabel, overlay->tpLabel, false);
  COIBoardAddString(board, overlay->atkLabel, 0);
  
  overlay->defLabel = COIStringCreate("DEF:", 0, 0, textType);
  COIStringConfineToSprite(overlay->defLabel, overlay->statWindow);
  COIStringPositionBelowString(overlay->defLabel, overlay->atkLabel, false);
  COIBoardAddString(board, overlay->defLabel, 0);
  
  overlay->agiLabel = COIStringCreate("AGI:", 0, 0, textType);
  COIStringConfineToSprite(overlay->agiLabel, overlay->statWindow);
  COIStringPositionBelowString(overlay->agiLabel, overlay->defLabel, false);
  COIBoardAddString(board, overlay->agiLabel, 0);

  // overlay->goldLabel = COIStringCreate("Gold:", 0, 0, textType);
  // COIStringConfineToSprite(overlay->goldLabel, overlay->statWindow);
  // COIStringPositionBelowString(overlay->goldLabel, overlay->agiLabel, false);
  // COIBoardAddString(board, overlay->goldLabel);

  _makeStatStrings(overlay);
}

static COIMenu* _makeTopRightMenu(COIBoard* board) {
  COISprite* frame = COISpriteCreateFromAssetID(320, 40, 250, 250,
						COI_GLOBAL_LOADER,
						5,
						COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  frame->_autoHandle = false;
  COIBoardAddDynamicSprite(board, frame, 0);
  COISprite* pointer = COISpriteCreateFromAssetID(0, 0, 32, 32,
						  COI_GLOBAL_LOADER,
						  6,
						  COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  pointer->_autoHandle = false;
  COIBoardAddDynamicSprite(board, pointer, 0);

  return COIMenuCreateWithCapacity(frame, pointer, BACKPACK_SIZE);
}

static void _makeBaseMenu(PauseOverlay* overlay, COITextType* textType, COIBoard* board) {
  overlay->baseMenu = _makeTopRightMenu(board);
  char* baseMenuOptions[] = { "Items", "Weapons", "Armor", "Options", "Quit Game" };
  COIString* strings[5];
  for (int i = 0; i < 5; i++) {
    char* option = baseMenuOptions[i];
    strings[i] = COIStringCreate(option, 0, 0, textType);
    COIBoardAddString(board, strings[i], 0);
  }
  COIMenuSetTexts(overlay->baseMenu, strings, 5);
}

static void _makeItemsMenu(PauseOverlay* overlay, PlayerInfo* pInfo, COITextType* textType, COIBoard* board) {
  overlay->itemsMenu = _makeTopRightMenu(board);
  overlay->weaponsMenu = _makeTopRightMenu(board);
  overlay->armorMenu = _makeTopRightMenu(board);
  Inventory* inventory = pInfo->inventory;

  // Items in backpack
  for (int i = 0; i < inventory->numBackpackItems; i++) {
    Item* item = inventory->backpack[i];
    COIString* string = COIStringCreate(ItemListStringFromItemID(item->id), 0, 0, textType);
    COIBoardAddString(board, string, 0);

    if (item->type == CONSUMABLE) {
      // Use index in back as returned value in menu.
      COIMenuAddString(overlay->itemsMenu, string, i);
    } else if (item->type == WEAPON) {
      COIMenuAddString(overlay->weaponsMenu, string, i);
    } else {
      COIMenuAddString(overlay->armorMenu, string, i);
    }
  }

  // Equipped items
  char temp[MAX_STRING_SIZE];
  if (inventory->head->id != ITEM_ID_UNARMORED_HEAD) {
    snprintf(temp, MAX_STRING_SIZE, "%s (e)", ItemListStringFromItemID(inventory->head->id));
    COIString* string = COIStringCreate(temp, 0, 0, textType);
    COIBoardAddString(board, string, 0);
    COIMenuAddString(overlay->armorMenu, string, PAUSE_OVERLAY_HEAD);
  }
  if (inventory->body->id != ITEM_ID_UNARMORED_BODY) {
    snprintf(temp, MAX_STRING_SIZE, "%s (e)", ItemListStringFromItemID(inventory->body->id));
    COIString* string = COIStringCreate(temp, 0, 0, textType);
    COIBoardAddString(board, string, 0);
    COIMenuAddString(overlay->armorMenu, string, PAUSE_OVERLAY_BODY);
  }
  if (inventory->legs->id != ITEM_ID_UNARMORED_LEGS) {
    snprintf(temp, MAX_STRING_SIZE, "%s (e)", ItemListStringFromItemID(inventory->legs->id));
    COIString* string = COIStringCreate(temp, 0, 0, textType);    
    COIBoardAddString(board, string, 0);
    COIMenuAddString(overlay->armorMenu, string, PAUSE_OVERLAY_LEGS);
  }
  if (inventory->weapon->id != ITEM_ID_UNARMED) {
    snprintf(temp, MAX_STRING_SIZE, "%s (e)", ItemListStringFromItemID(inventory->weapon->id));
    COIString* string = COIStringCreate(temp, 0, 0, textType);        
    COIBoardAddString(board, string, 0);
    COIMenuAddString(overlay->weaponsMenu, string, PAUSE_OVERLAY_WEAPON);    
  }
  if (inventory->offHand->id != ITEM_ID_UNARMED_OFF) {
    snprintf(temp, MAX_STRING_SIZE, "%s (e)", ItemListStringFromItemID(inventory->offHand->id));
    COIString* string = COIStringCreate(temp, 0, 0, textType);    
    COIBoardAddString(board, string, 0);
    if (inventory->offHand->type == WEAPON) {
      COIMenuAddString(overlay->weaponsMenu, string, PAUSE_OVERLAY_OFFHAND);    
    } else {
      COIMenuAddString(overlay->armorMenu, string, PAUSE_OVERLAY_OFFHAND);    
    }
  }  
}

static void _makeConfirmMenus(PauseOverlay* overlay) {
  COIString* primary;
  COIString* cancel;

  overlay->useItemMenu = _makeTopRightMenu(overlay->board);
  primary = COIStringCreate("Use\0", 0, 0, overlay->textType);
  COIBoardAddString(overlay->board, primary, 0);
  COIMenuAddString(overlay->useItemMenu, primary, 0);
  cancel = COIStringCreate("Cancel\0", 0, 0, overlay->textType);
  COIBoardAddString(overlay->board, cancel, 0);
  COIMenuAddString(overlay->useItemMenu, cancel, 1);

  overlay->equipMenu = _makeTopRightMenu(overlay->board);
  primary = COIStringCreate("Equip\0", 0, 0, overlay->textType);
  COIBoardAddString(overlay->board, primary, 0);
  COIMenuAddString(overlay->equipMenu, primary, 0);
  cancel = COIStringCreate("Cancel\0", 0, 0, overlay->textType);
  COIBoardAddString(overlay->board, cancel, 0);
  COIMenuAddString(overlay->equipMenu, cancel, 1);

  overlay->unequipMenu = _makeTopRightMenu(overlay->board);
  primary = COIStringCreate("Unequip\0", 0, 0, overlay->textType);
  COIBoardAddString(overlay->board, primary, 0);
  COIMenuAddString(overlay->unequipMenu, primary, 0);
  cancel = COIStringCreate("Cancel\0", 0, 0, overlay->textType);
  COIBoardAddString(overlay->board, cancel, 0);
  COIMenuAddString(overlay->unequipMenu, cancel, 1);

  overlay->quitMenu = _makeTopRightMenu(overlay->board);
  primary = COIStringCreate("I'm Sure\0", 0, 0, overlay->textType);
  COIBoardAddString(overlay->board, primary, 0);
  COIMenuAddString(overlay->quitMenu, primary, 0);
  cancel = COIStringCreate("Cancel\0", 0, 0, overlay->textType);
  COIBoardAddString(overlay->board, cancel, 0);
  COIMenuAddString(overlay->quitMenu, cancel, 1);
}


static void _makeGearWindow(PauseOverlay* overlay, PlayerInfo* pInfo, COITextType* textType, COIBoard* board) {
  overlay->gearWindow = COISpriteCreateFromAssetID(70, 290, 500, 150,
						   COI_GLOBAL_LOADER,
						   5,
						   COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  overlay->gearWindow->_autoHandle = false;
  COIBoardAddDynamicSprite(board, overlay->gearWindow, 0);


  Inventory* inventory = pInfo->inventory;
  char temp[MAX_STRING_SIZE];

  overlay->weaponLabel = COIStringCreate("WEAPON:", 0, 0, textType);
  COIStringConfineToSprite(overlay->weaponLabel, overlay->gearWindow);
  COIBoardAddString(board, overlay->weaponLabel, 0);
  overlay->weapon = COIStringCreate(ItemListStringFromItemID(inventory->weapon->id), 0, 0, textType);
  COIStringConfineToSprite(overlay->weapon, overlay->gearWindow);
  COIStringPositionRightOfString(overlay->weapon, overlay->weaponLabel, 50);
  COIBoardAddString(board, overlay->weapon, 0);
  
  overlay->offHandLabel = COIStringCreate("OFF-HAND:", 0, 0, textType);
  COIStringConfineToSprite(overlay->offHandLabel, overlay->gearWindow);
  COIStringPositionBelowString(overlay->offHandLabel, overlay->weaponLabel, false);
  COIBoardAddString(board, overlay->offHandLabel, 0);
  overlay->offHand = COIStringCreate(ItemListStringFromItemID(inventory->offHand->id), 0, 0, textType);
  COIStringPositionBelowString(overlay->offHand, overlay->weapon, false);
  COIBoardAddString(board, overlay->offHand, 0);

  overlay->headLabel = COIStringCreate("HEAD:", 0, 0, textType);
  COIStringConfineToSprite(overlay->headLabel, overlay->gearWindow);
  COIStringPositionBelowString(overlay->headLabel, overlay->offHandLabel, false);
  COIBoardAddString(board, overlay->headLabel, 0);
  overlay->head = COIStringCreate(ItemListStringFromItemID(inventory->head->id), 0, 0, textType);
  COIStringPositionBelowString(overlay->head, overlay->offHand, false);
  COIBoardAddString(board, overlay->head, 0);

  overlay->bodyLabel = COIStringCreate("BODY:", 0, 0, textType);
  COIStringConfineToSprite(overlay->bodyLabel, overlay->gearWindow);
  COIStringPositionBelowString(overlay->bodyLabel, overlay->headLabel, false);
  COIBoardAddString(board, overlay->bodyLabel, 0);
  overlay->body = COIStringCreate(ItemListStringFromItemID(inventory->body->id), 0, 0, textType);
  COIStringPositionBelowString(overlay->body, overlay->head, false);
  COIBoardAddString(board, overlay->body, 0);

  overlay->legsLabel = COIStringCreate("LEGS:", 0, 0, textType);
  COIStringConfineToSprite(overlay->legsLabel, overlay->gearWindow);
  COIStringPositionBelowString(overlay->legsLabel, overlay->bodyLabel, false);
  COIBoardAddString(board, overlay->legsLabel, 0);
  overlay->legs = COIStringCreate(ItemListStringFromItemID(inventory->legs->id), 0, 0, textType);
  COIStringPositionBelowString(overlay->legs, overlay->body, false);
  COIBoardAddString(board, overlay->legs, 0);
}

static void _destroyStatStrings(PauseOverlay* overlay) {
  COIBoardRemoveString(overlay->board, overlay->atk, 0);
  COIStringDestroy(overlay->atk);
  overlay->atk = NULL;
  COIBoardRemoveString(overlay->board, overlay->def, 0);
  COIStringDestroy(overlay->def);
  overlay->def = NULL;
  COIBoardRemoveString(overlay->board, overlay->agi, 0);
  COIStringDestroy(overlay->agi);
  overlay->agi = NULL;
  COIBoardRemoveString(overlay->board, overlay->hp, 0);
  COIStringDestroy(overlay->hp);
  overlay->hp = NULL;
  COIBoardRemoveString(overlay->board, overlay->sp, 0);
  COIStringDestroy(overlay->sp);
  overlay->sp = NULL;
  COIBoardRemoveString(overlay->board, overlay->tp, 0);
  COIStringDestroy(overlay->tp);
  overlay->tp = NULL;
  COIBoardRemoveString(overlay->board, overlay->gold, 0);
  COIStringDestroy(overlay->gold);
  overlay->gold = NULL;
}

static void _updateGearString(PauseOverlay* overlay, int slot) {
  COIString* oldString;
  Inventory* inventory = overlay->pInfo->inventory;
  
  switch (slot) {
  case ITEM_SLOT_WEAPON:
    oldString = overlay->weapon;
    overlay->weapon = COIStringCreate(ItemListStringFromItemID(inventory->weapon->id), oldString->x, oldString->y, overlay->textType);
    COIBoardAddString(overlay->board, overlay->weapon, 0);
    break;
  case ITEM_SLOT_OFFHAND:
    oldString = overlay->offHand;
    overlay->offHand = COIStringCreate(ItemListStringFromItemID(inventory->offHand->id), oldString->x, oldString->y, overlay->textType);
    COIBoardAddString(overlay->board, overlay->offHand, 0);
    break;
  case ITEM_SLOT_HEAD:
    oldString = overlay->head;
    overlay->head = COIStringCreate(ItemListStringFromItemID(inventory->head->id), oldString->x, oldString->y, overlay->textType);
    COIBoardAddString(overlay->board, overlay->head, 0);
    break;
  case ITEM_SLOT_BODY:
    oldString = overlay->body;
    overlay->body = COIStringCreate(ItemListStringFromItemID(inventory->body->id), oldString->x, oldString->y, overlay->textType);
    COIBoardAddString(overlay->board, overlay->body, 0);
    break;
  case ITEM_SLOT_LEGS:
    oldString = overlay->legs;
    overlay->legs = COIStringCreate(ItemListStringFromItemID(inventory->legs->id), oldString->x, oldString->y, overlay->textType);
    COIBoardAddString(overlay->board, overlay->legs, 0);
    break;
  default:
    printf("Error: invalid slot type in pause overlay.\n");
  }
  
  COIBoardRemoveString(overlay->board, oldString, 0);
  COIStringDestroy(oldString);
}

static void _returnToItemsMenu(PauseOverlay* overlay) {
  COIMenuSetInvisible(overlay->topRightMenu);
  overlay->topRightMenu = overlay->itemsMenu;
  COIMenuSetVisible(overlay->topRightMenu);
}

static bool _baseMenuSelect(PauseOverlay* overlay) {
  bool validSelection = false;
  COIMenuSetInvisible(overlay->baseMenu);
  switch (overlay->topRightMenu->_current) {
  case PAUSE_OVERLAY_ITEMS:
    if (overlay->itemsMenu->_stringCount > 0) {
      overlay->topRightMenu = overlay->itemsMenu;
      validSelection = true;
    }
    break;
  case PAUSE_OVERLAY_WEAPONS:
    if (overlay->weaponsMenu->_stringCount > 0) {
      overlay->topRightMenu = overlay->weaponsMenu;
      _updateStatChanges(overlay, COIMenuGetCurrentValue(overlay->topRightMenu));
      validSelection = true;
    }
    break;
  case PAUSE_OVERLAY_ARMOR:
    if (overlay->armorMenu->_stringCount > 0) {
      overlay->topRightMenu = overlay->armorMenu;
      _updateStatChanges(overlay, COIMenuGetCurrentValue(overlay->topRightMenu));
      validSelection = true;
    }
    break;
  case PAUSE_OVERLAY_OPTIONS:
    COIPreferencesMenuSetVisible(&overlay->prefMenu, true);
    break;
  case PAUSE_OVERLAY_QUIT:
    overlay->topRightMenu = overlay->quitMenu;
    validSelection = true;
    break;
  default:
    overlay->topRightMenu = overlay->baseMenu;
    validSelection = true;
  }
  COIMenuSetVisible(overlay->topRightMenu);
  return validSelection;
}

static void _subMenuSelect(PauseOverlay* overlay) {
  COIMenuSetInvisible(overlay->topRightMenu);
  
  Inventory* inventory = overlay->pInfo->inventory;
  int itemIndex = COIMenuGetCurrentValue(overlay->topRightMenu);
  bool equipped;

  // If index is negative, then it's an equipped item
  if (itemIndex >= 0) {
    equipped = false;
    overlay->selectedItem = inventory->backpack[itemIndex];
  } else {
    equipped = true;
    switch (itemIndex) {
    case PAUSE_OVERLAY_WEAPON:
      overlay->selectedItem = inventory->weapon;
      break;
    case PAUSE_OVERLAY_OFFHAND:
      overlay->selectedItem = inventory->offHand;
      break;
    case PAUSE_OVERLAY_HEAD:
      overlay->selectedItem = inventory->head;
      break;
    case PAUSE_OVERLAY_BODY:
      overlay->selectedItem = inventory->body;
      break;
    case PAUSE_OVERLAY_LEGS:
      overlay->selectedItem = inventory->legs;
      break;
    }
  }

  // Display confirm menu depending on item type
  if (overlay->selectedItem->type == CONSUMABLE) {
    overlay->topRightMenu = overlay->useItemMenu;
  } else if (equipped) {
    overlay->topRightMenu = overlay->unequipMenu;
  } else {
    overlay->topRightMenu = overlay->equipMenu;
  }

  COIMenuSetVisible(overlay->topRightMenu);
}

static void _returnToBaseMenu(PauseOverlay* overlay) {
  COIMenuSetInvisible(overlay->topRightMenu);
  overlay->topRightMenu = overlay->baseMenu;
  COIMenuSetVisible(overlay->topRightMenu);
}

static void _quitMenuSelect(PauseOverlay* overlay) {
  if (COIMenuGetCurrentValue(overlay->topRightMenu) == 0) {
    COI_GLOBAL_WINDOW->shouldQuit = true;
  } else {
    _returnToBaseMenu(overlay);
  }
}

static void _returnToEquipableMenu(PauseOverlay* overlay) {
  COIMenuSetInvisible(overlay->topRightMenu);
  if (overlay->selectedItem->type == ARMOR) {
    overlay->topRightMenu = overlay->armorMenu;
  } else {
    overlay->topRightMenu = overlay->weaponsMenu;
  }
  _updateStatChanges(overlay, COIMenuGetCurrentValue(overlay->topRightMenu));  
  COIMenuSetVisible(overlay->topRightMenu);
}

static void _equipMenuSelect(PauseOverlay* overlay) {
  if (COIMenuGetCurrentValue(overlay->topRightMenu) == 0) {
    Inventory* inventory = overlay->pInfo->inventory;
    // Add associated SPECIAL if we're removing Tagnesse
    if (overlay->selectedItem->id == ITEM_ID_TAGNESSE) {
      IntListAdd(&overlay->pInfo->party[0]->specials, SPECIAL_ID_TIME_SKIP);
    }
    inventoryRemoveBackpackItemFirstInstance(inventory, overlay->selectedItem);
    Item* oldItem = inventoryEquipItem(inventory, overlay->selectedItem);
    inventoryAddItem(inventory, oldItem->id);

    _destroySubMenus(overlay);
    _makeItemsMenu(overlay, overlay->pInfo, overlay->textType, overlay->board);
    COIMenuSetInvisible(overlay->itemsMenu);
    COIMenuSetInvisible(overlay->weaponsMenu);
    COIMenuSetInvisible(overlay->armorMenu);
    _destroyStatStrings(overlay);
    _makeStatStrings(overlay);
    _updateGearString(overlay, overlay->selectedItem->slot);
    // A bit hacky with the second argument, but we know we want to turn the stat strings off.
    _updateStatChanges(overlay, -1);
  }
  
  _returnToEquipableMenu(overlay);
}

static void _unequipMenuSelect(PauseOverlay* overlay) {
  if (COIMenuGetCurrentValue(overlay->topRightMenu) == 0) {
    Inventory* inventory = overlay->pInfo->inventory;
    // Remove associated SPECIAL if we're removing Tagnesse
    if (overlay->selectedItem->id == ITEM_ID_TAGNESSE) {
      IntList* specialsPtr = &overlay->pInfo->party[0]->specials;
      int timeSkipIndex = -1;
      for (int i = 0; i < specialsPtr->length; i++) {
        if (specialsPtr->values[i] == SPECIAL_ID_TIME_SKIP) {
          timeSkipIndex = i;
          break;
        }
      }
      IntListDelete(&overlay->pInfo->party[0]->specials, timeSkipIndex);
    }
    inventoryRemoveEquippedItem(inventory, overlay->selectedItem->slot);
    inventoryAddItem(inventory, overlay->selectedItem->id);

    _destroySubMenus(overlay);
    _makeItemsMenu(overlay, overlay->pInfo, overlay->textType, overlay->board);
    COIMenuSetInvisible(overlay->itemsMenu);
    COIMenuSetInvisible(overlay->weaponsMenu);
    COIMenuSetInvisible(overlay->armorMenu);
    _destroyStatStrings(overlay);
    _makeStatStrings(overlay);
    _updateGearString(overlay, overlay->selectedItem->slot);
    _updateStatChanges(overlay, -1);
  }

  _returnToEquipableMenu(overlay);
}

static void _itemsMenuSelect(PauseOverlay* overlay, TextBox* textBox) {
  bool overlayInvisible = false;
  if (COIMenuGetCurrentValue(overlay->topRightMenu) == 0) {
    Actor* player = overlay->pInfo->party[0];
    Inventory* inventory = overlay->pInfo->inventory;
    // Special case
    if (overlay->selectedItem->id == ITEM_ID_GEM_OF_PERMANENCE) {
      TextBoxSetStrings(textBox, "Game saved.", NULL);
      inventoryRemoveBackpackItemFirstInstance(inventory, overlay->selectedItem);
      playerEncode(overlay->pInfo);
      overlayInvisible = true;
    } else {
      actorUseConsumable(player, overlay->selectedItem);
      inventoryRemoveBackpackItemFirstInstance(inventory, overlay->selectedItem);
    }

    _destroySubMenus(overlay);
    _makeItemsMenu(overlay, overlay->pInfo, overlay->textType, overlay->board);
    COIMenuSetInvisible(overlay->itemsMenu);
    COIMenuSetInvisible(overlay->weaponsMenu);
    COIMenuSetInvisible(overlay->armorMenu);
    _destroyStatStrings(overlay);
    _makeStatStrings(overlay);
  }

  if (overlayInvisible) {
    PauseOverlaySetVisible(overlay, false);
  } else if (overlay->itemsMenu->_stringCount > 0) {
    _returnToItemsMenu(overlay);
  } else {
    _returnToBaseMenu(overlay);
  }
  
}



PauseOverlay* PauseOverlayCreate(PlayerInfo* pInfo, COITextType* textType, COIBoard* board) {
  // May want to have several separate boxes, not just the single big one
  PauseOverlay* overlay = malloc(sizeof(PauseOverlay));
  overlay->dirty = false;
  overlay->textType = textType;
  overlay->board = board;
  overlay->pInfo = pInfo;
  overlay->selectedItem = NULL;
  overlay->atkChange = NULL;
  overlay->defChange = NULL;
  overlay->agiChange = NULL;
  overlay->hpChange = NULL;
  overlay->spChange = NULL;
  overlay->tpChange = NULL;
  overlay->goldLabel = NULL;
  overlay->gold = NULL;
  COIPreferencesMenuInit(&overlay->prefMenu, board);
  COIPreferencesMenuSetVisible(&overlay->prefMenu, false);
 

  // Used to show how an item will affect the player's stats.
  overlay->posTextType = COITextTypeCreate(textType->fontSize,
						    0, 255, 0,
						    COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  overlay->negTextType = COITextTypeCreate(textType->fontSize,
						    255, 0, 0,
						    COIWindowGetRenderer(COI_GLOBAL_WINDOW));						    
						    

  _makeStatWindow(overlay, pInfo, textType, board);
  _makeBaseMenu(overlay, textType, board);
  _makeItemsMenu(overlay, pInfo, textType, board);
  _makeConfirmMenus(overlay);
  _makeGearWindow(overlay, pInfo, textType, board);

  overlay->topRightMenu = overlay->baseMenu;

  PauseOverlaySetVisible(overlay, false);
  
  return overlay;
}

void PauseOverlaySelect(PauseOverlay* overlay, TextBox* textBox) {
  // True if the 'select' event causes something to happen.
  // Used to determine what sound we play on selection
  bool validSelection = true;

  if (overlay->prefMenu.frame->_visible) {
    COIPreferencesMenuProcessInput(&overlay->prefMenu, MOVING_SELECT);
    // Did we close the preferences menu?
    if (!overlay->prefMenu.frame->_visible) {
      PauseOverlaySetVisible(overlay, false);
      COIBoardQueueDraw(overlay->board);
    }
    return;
  }
  
  if (overlay->topRightMenu == overlay->baseMenu) {
    validSelection = _baseMenuSelect(overlay);
  } else if (overlay->topRightMenu == overlay->itemsMenu ||
	     overlay->topRightMenu == overlay->weaponsMenu ||
	     overlay->topRightMenu == overlay->armorMenu) {
    _subMenuSelect(overlay);
  } else if (overlay->topRightMenu == overlay->quitMenu) {
    _quitMenuSelect(overlay);
  } else if (overlay->topRightMenu == overlay->equipMenu) {
    _equipMenuSelect(overlay);
  } else if (overlay->topRightMenu == overlay->unequipMenu) {
    _unequipMenuSelect(overlay);
  } else {
    _itemsMenuSelect(overlay, textBox);
  }
  if (validSelection) {
    COISoundPlay(COI_SOUND_SELECT);
  } else {
    COISoundPlay(COI_SOUND_INVALID);
  }
}

void PauseOverlayBack(PauseOverlay* overlay) {
  COISoundPlay(COI_SOUND_INVALID);
  if ((overlay->topRightMenu == overlay->equipMenu) ||
      (overlay->topRightMenu == overlay->unequipMenu)) {
    _returnToEquipableMenu(overlay);
  } else if (overlay->topRightMenu != overlay->baseMenu) {
    _returnToBaseMenu(overlay);
    _updateStatChanges(overlay, -1);
  }
}


static void _destroyConfirmMenus(PauseOverlay* overlay) {
  COIMenuDestroyAndFreeComponents(overlay->useItemMenu, overlay->board);
  COIMenuDestroyAndFreeComponents(overlay->equipMenu, overlay->board);
  COIMenuDestroyAndFreeComponents(overlay->unequipMenu, overlay->board);
  COIMenuDestroyAndFreeComponents(overlay->quitMenu, overlay->board);
}


void PauseOverlayDestroy(PauseOverlay* overlay, COIBoard* board) {
  COIPreferencesMenuDeinit(&overlay->prefMenu, board);
  // Stat Window
  COIBoardRemoveString(board, overlay->name, 0);
  COIStringDestroy(overlay->name);
  COIBoardRemoveString(board, overlay->class, 0);
  COIStringDestroy(overlay->class);
  COIBoardRemoveString(board, overlay->lvLabel, 0);
  COIStringDestroy(overlay->lvLabel);
  COIBoardRemoveString(board, overlay->lv, 0);
  COIStringDestroy(overlay->lv);
  COIBoardRemoveString(board, overlay->hpLabel, 0);
  COIStringDestroy(overlay->hpLabel);
  COIBoardRemoveString(board, overlay->spLabel, 0);
  COIStringDestroy(overlay->spLabel);
  COIBoardRemoveString(board, overlay->tpLabel, 0);
  COIStringDestroy(overlay->tpLabel);
  COIBoardRemoveString(board, overlay->atkLabel, 0);
  COIStringDestroy(overlay->atkLabel);
  COIBoardRemoveString(board, overlay->agiLabel, 0);
  COIStringDestroy(overlay->agiLabel);
  COIBoardRemoveString(board, overlay->defLabel, 0);
  COIStringDestroy(overlay->defLabel);
  COIBoardRemoveString(board, overlay->goldLabel, 0);
  COIStringDestroy(overlay->goldLabel);
  _destroyStatStrings(overlay);
  COIBoardRemoveDynamicSprite(board, overlay->statWindow, 0);
  COISpriteDestroy(overlay->statWindow);

  COIBoardRemoveString(board, overlay->headLabel, 0);
  COIStringDestroy(overlay->headLabel);
  COIBoardRemoveString(board, overlay->head, 0);
  COIStringDestroy(overlay->head);
  COIBoardRemoveString(board, overlay->bodyLabel, 0);
  COIStringDestroy(overlay->bodyLabel);
  COIBoardRemoveString(board, overlay->body, 0);
  COIStringDestroy(overlay->body);
  COIBoardRemoveString(board, overlay->legsLabel, 0);
  COIStringDestroy(overlay->legsLabel);
  COIBoardRemoveString(board, overlay->legs, 0);
  COIStringDestroy(overlay->legs);
  COIBoardRemoveString(board, overlay->weaponLabel, 0);
  COIStringDestroy(overlay->weaponLabel);
  COIBoardRemoveString(board, overlay->weapon, 0);
  COIStringDestroy(overlay->weapon);
  COIBoardRemoveString(board, overlay->offHandLabel, 0);
  COIStringDestroy(overlay->offHandLabel);
  COIBoardRemoveString(board, overlay->offHand, 0);
  COIStringDestroy(overlay->offHand);  
  COIBoardRemoveDynamicSprite(board, overlay->gearWindow, 0);
  COISpriteDestroy(overlay->gearWindow);

  COIMenuDestroyAndFreeComponents(overlay->baseMenu, board);
  _destroySubMenus(overlay);
  _destroyConfirmMenus(overlay);

  _removeStringIfExists(overlay, overlay->atkChange);
  _removeStringIfExists(overlay, overlay->defChange);
  _removeStringIfExists(overlay, overlay->agiChange);
  _removeStringIfExists(overlay, overlay->hpChange);
  _removeStringIfExists(overlay, overlay->spChange);
  _removeStringIfExists(overlay, overlay->tpChange);

  COITextTypeDestroy(overlay->posTextType);
  COITextTypeDestroy(overlay->negTextType);

  free(overlay);
}

void PauseOverlaySetVisible(PauseOverlay* overlay, bool visible) {
  COIStringSetVisible(overlay->name, visible);
  COIStringSetVisible(overlay->class, visible);
  COIStringSetVisible(overlay->lvLabel, visible);
  COIStringSetVisible(overlay->lv, visible);
  COIStringSetVisible(overlay->hpLabel, visible);
  COIStringSetVisible(overlay->hp, visible);
  COIStringSetVisible(overlay->spLabel, visible);
  COIStringSetVisible(overlay->sp, visible);
  COIStringSetVisible(overlay->tpLabel, visible);
  COIStringSetVisible(overlay->tp, visible);
  COIStringSetVisible(overlay->atkLabel, visible);
  COIStringSetVisible(overlay->atk, visible);
  COIStringSetVisible(overlay->agiLabel, visible);
  COIStringSetVisible(overlay->agi, visible);
  COIStringSetVisible(overlay->defLabel, visible);
  COIStringSetVisible(overlay->def, visible);
  COIStringSetVisible(overlay->goldLabel, visible);
  COIStringSetVisible(overlay->gold, visible);
  overlay->statWindow->_visible = visible;

  COIStringSetVisible(overlay->headLabel, visible);
  COIStringSetVisible(overlay->head, visible);
  COIStringSetVisible(overlay->bodyLabel, visible);
  COIStringSetVisible(overlay->body, visible);
  COIStringSetVisible(overlay->legsLabel, visible);
  COIStringSetVisible(overlay->legs, visible);
  COIStringSetVisible(overlay->weaponLabel, visible);
  COIStringSetVisible(overlay->weapon, visible);
  COIStringSetVisible(overlay->offHandLabel, visible);
  COIStringSetVisible(overlay->offHand, visible);
  overlay->gearWindow->_visible = visible;
  
  if (visible) {
    COIMenuSetVisible(overlay->baseMenu);
  } else {
    COIMenuSetInvisible(overlay->baseMenu);
  }

  if (overlay->dirty) {
    _destroySubMenus(overlay);
    _makeItemsMenu(overlay, overlay->pInfo, overlay->textType, overlay->board);

    // Update gear
    _updateGearString(overlay, ITEM_SLOT_WEAPON);
    _updateGearString(overlay, ITEM_SLOT_OFFHAND);
    _updateGearString(overlay, ITEM_SLOT_HEAD);
    _updateGearString(overlay, ITEM_SLOT_BODY);
    _updateGearString(overlay, ITEM_SLOT_LEGS);
    overlay->dirty = false;

    // Update stats
    _destroyStatStrings(overlay);
    _makeStatStrings(overlay);

    // Remake the level string
    COIBoardRemoveString(overlay->board, overlay->lv, 0);
    COIStringDestroy(overlay->lv);
    _makeLevelString(overlay);

    // Remake class string
    COIBoardRemoveString(overlay->board, overlay->class, 0);
    COIStringDestroy(overlay->class);
    _makeClassString(overlay);
  }

  // Secondary menus always start out invisible
  COIMenuSetInvisible(overlay->itemsMenu);
  COIMenuSetInvisible(overlay->weaponsMenu);
  COIMenuSetInvisible(overlay->armorMenu);
  COIMenuSetInvisible(overlay->useItemMenu);
  COIMenuSetInvisible(overlay->equipMenu);
  COIMenuSetInvisible(overlay->unequipMenu);
  COIMenuSetInvisible(overlay->quitMenu);

  // Stat change string starts out invisible because we don't start out looking at items.
  _setStringVisibleIfExists(overlay->atkChange, false);
  _setStringVisibleIfExists(overlay->defChange, false);
  _setStringVisibleIfExists(overlay->agiChange, false);
  _setStringVisibleIfExists(overlay->hpChange, false);
  _setStringVisibleIfExists(overlay->spChange, false);
  _setStringVisibleIfExists(overlay->tpChange, false);

  overlay->visible = visible;
  overlay->topRightMenu = overlay->baseMenu;
}

void PauseOverlayToggleVisibility(PauseOverlay* overlay) {
  if (overlay->prefMenu.frame->_visible) {
    COIPreferencesMenuProcessInput(&overlay->prefMenu, MOVING_PAUSE);
  } else {
    PauseOverlaySetVisible(overlay, !overlay->visible);
  }
}


void PauseOverlayProcessInput(PauseOverlay* overlay, int event) {
  if (overlay->prefMenu.frame->_visible) {
    COIPreferencesMenuProcessInput(&overlay->prefMenu, event);
    return;
  } 

  if (event == MOVING_DELETE) {
    PauseOverlayBack(overlay);
  } else {
    int valueBefore = COIMenuGetCurrentValue(overlay->topRightMenu);
    COIMenuHandleInput(overlay->topRightMenu, event, true);
    int valueAfter = COIMenuGetCurrentValue(overlay->topRightMenu);
  
    // Only update stat change string if we're in the weapons or armor menu. Only these types of items
    // affect stats.
    // Also only update stat change string if we've moved to a different item in the menu.
    if ((overlay->topRightMenu == overlay->weaponsMenu ||
	 overlay->topRightMenu == overlay->armorMenu) &&
	(valueBefore != valueAfter)) {
      _updateStatChanges(overlay, valueAfter);
    }
  }
}
