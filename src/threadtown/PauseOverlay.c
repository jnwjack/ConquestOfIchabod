#include "PauseOverlay.h"

static void _removeStringIfExists(PauseOverlay* overlay, COIString* string) {
  if (string != NULL) {
    COIBoardRemoveString(overlay->board, string);
    COIStringDestroy(string);
  }
}

static void _setStringVisibleIfExists(COIString* string, bool visible) {
  if (string != NULL) {
    COIStringSetVisible(string, visible);
  }
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

    // We should only display a stat change string if there's an actual stat change.
    if (newHP != oldHP) {
      _removeStringIfExists(overlay, overlay->hpChange);
      snprintf(temp, MAX_STRING_SIZE, "%i", newHP);
      overlay->atkChange = COIStringCreate(temp, 0, 0,
					   newHP > oldHP ? overlay->posTextType : overlay->negTextType);
      COIStringConfineToSprite(overlay->hpChange, overlay->statWindow);
      COIStringPositionBelowString(overlay->hpChange, overlay->class);
      COIStringPositionRightOfString(overlay->hpChange, overlay->hp, 10);
      COIBoardAddString(overlay->board, overlay->hpChange);
      _setStringVisibleIfExists(overlay->hpChange, true);      
    } else {
      _setStringVisibleIfExists(overlay->hpChange, false);
    }

    if (newSP != oldSP) {
      _removeStringIfExists(overlay, overlay->spChange);
      snprintf(temp, MAX_STRING_SIZE, "%i", newSP);
      overlay->atkChange = COIStringCreate(temp, 0, 0,
					   newSP > oldSP ? overlay->posTextType : overlay->negTextType);
      COIStringConfineToSprite(overlay->spChange, overlay->statWindow);
      COIStringPositionBelowString(overlay->spChange, overlay->hp);
      COIStringPositionRightOfString(overlay->spChange, overlay->sp, 10);
      COIBoardAddString(overlay->board, overlay->spChange);
      _setStringVisibleIfExists(overlay->spChange, true);      
    } else {
      _setStringVisibleIfExists(overlay->spChange, false);
    }

    if (newTP != oldTP) {
      _removeStringIfExists(overlay, overlay->tpChange);
      snprintf(temp, MAX_STRING_SIZE, "%i", newTP);
      overlay->atkChange = COIStringCreate(temp, 0, 0,
					   newTP > oldTP ? overlay->posTextType : overlay->negTextType);
      COIStringConfineToSprite(overlay->tpChange, overlay->statWindow);
      COIStringPositionBelowString(overlay->tpChange, overlay->sp);
      COIStringPositionRightOfString(overlay->tpChange, overlay->tp, 10);
      COIBoardAddString(overlay->board, overlay->tpChange);
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
      COIStringPositionBelowString(overlay->atkChange, overlay->tp);
      COIStringPositionRightOfString(overlay->atkChange, overlay->atk, 10);
      COIBoardAddString(overlay->board, overlay->atkChange);
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
      COIStringPositionBelowString(overlay->defChange, overlay->atk);
      COIStringPositionRightOfString(overlay->defChange, overlay->def, 10);
      COIBoardAddString(overlay->board, overlay->defChange);
      _setStringVisibleIfExists(overlay->defChange, true);
    } else {
      _setStringVisibleIfExists(overlay->defChange, false);
    }

    if (newAGI != oldAGI) {
      _removeStringIfExists(overlay, overlay->agiChange);
      snprintf(temp, MAX_STRING_SIZE, "%i", newAGI);
      overlay->defChange = COIStringCreate(temp, 0, 0,
					   newAGI > oldAGI ? overlay->posTextType : overlay->negTextType);
      COIStringConfineToSprite(overlay->agiChange, overlay->statWindow);
      COIStringPositionBelowString(overlay->agiChange, overlay->def);
      COIStringPositionRightOfString(overlay->agiChange, overlay->agi, 10);
      COIBoardAddString(overlay->board, overlay->defChange);
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


static void _makeStatStrings(PauseOverlay* overlay) {
  Actor* player = overlay->pInfo->party[0];
  char temp[MAX_STRING_SIZE];
  
  snprintf(temp, MAX_STRING_SIZE, "%i/%i", player->hp, player->hpMax);  
  overlay->hp = COIStringCreate(temp, 0, 0, overlay->textType);
  COIStringPositionBelowString(overlay->hp, overlay->class);
  COIStringPositionRightOfString(overlay->hp, overlay->hpLabel, 50);
  COIBoardAddString(overlay->board, overlay->hp);

  snprintf(temp, MAX_STRING_SIZE, "%i/%i", player->sp, player->spMax);
  overlay->sp = COIStringCreate(temp, 0, 0, overlay->textType);
  COIStringPositionBelowString(overlay->sp, overlay->hp);
  COIBoardAddString(overlay->board, overlay->sp);

  snprintf(temp, MAX_STRING_SIZE, "%i/%i", player->tp, player->tpMax);
  overlay->tp = COIStringCreate(temp, 0, 0, overlay->textType);
  COIStringPositionBelowString(overlay->tp, overlay->sp);
  COIBoardAddString(overlay->board, overlay->tp);

  snprintf(temp, MAX_STRING_SIZE, "%i", playerAdjustedATK(overlay->pInfo));
  overlay->atk = COIStringCreate(temp, 0, 0, overlay->textType);
  COIStringPositionBelowString(overlay->atk, overlay->tp);
  COIBoardAddString(overlay->board, overlay->atk);

  snprintf(temp, MAX_STRING_SIZE, "%i", playerAdjustedDEF(overlay->pInfo));
  overlay->def = COIStringCreate(temp, 0, 0, overlay->textType);
  COIStringPositionBelowString(overlay->def, overlay->atk);
  COIBoardAddString(overlay->board, overlay->def);

  snprintf(temp, MAX_STRING_SIZE, "%i", player->agi);
  overlay->agi = COIStringCreate(temp, 0, 0, overlay->textType);
  COIStringPositionBelowString(overlay->agi, overlay->def);
  COIBoardAddString(overlay->board, overlay->agi);
}

static void _makeStatWindow(PauseOverlay* overlay, PlayerInfo* pInfo, COITextType* textType, COIBoard* board) {
  Actor* player = pInfo->party[0];
  
  overlay->statWindow = COISpriteCreateFromAssetID(70, 40, 250, 250,
						   COI_GLOBAL_LOADER,
						   5,
						   COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  overlay->statWindow->_autoHandle = true;
  COIBoardAddDynamicSprite(board, overlay->statWindow);
  overlay->name = COIStringCreate(pInfo->name, 0, 0, textType);
  overlay->class = COIStringCreate("Fighter", 0, 0, textType);
  COIStringConfineToSprite(overlay->name, overlay->statWindow);
  COIBoardAddString(board, overlay->name);
  COIStringConfineToSprite(overlay->class, overlay->statWindow);
  COIStringPositionBelowString(overlay->class, overlay->name);
  COIBoardAddString(board, overlay->class);

  char temp[MAX_STRING_SIZE];
  
  overlay->hpLabel = COIStringCreate("HP:", 0, 0, textType);
  COIStringConfineToSprite(overlay->hpLabel, overlay->statWindow);
  COIStringPositionBelowString(overlay->hpLabel, overlay->class);
  COIBoardAddString(board, overlay->hpLabel);

  overlay->spLabel = COIStringCreate("SP:", 0, 0, textType);
  COIStringConfineToSprite(overlay->spLabel, overlay->statWindow);
  COIStringPositionBelowString(overlay->spLabel, overlay->hpLabel);
  COIBoardAddString(board, overlay->spLabel);

  overlay->tpLabel = COIStringCreate("TP:", 0, 0, textType);
  COIStringConfineToSprite(overlay->tpLabel, overlay->statWindow);
  COIStringPositionBelowString(overlay->tpLabel, overlay->spLabel);
  COIBoardAddString(board, overlay->tpLabel);
  
  overlay->atkLabel = COIStringCreate("ATK:", 0, 0, textType);
  COIStringConfineToSprite(overlay->atkLabel, overlay->statWindow);
  COIStringPositionBelowString(overlay->atkLabel, overlay->tpLabel);
  COIBoardAddString(board, overlay->atkLabel);
  
  overlay->defLabel = COIStringCreate("DEF:", 0, 0, textType);
  COIStringConfineToSprite(overlay->defLabel, overlay->statWindow);
  COIStringPositionBelowString(overlay->defLabel, overlay->atkLabel);
  COIBoardAddString(board, overlay->defLabel);
  
  overlay->agiLabel = COIStringCreate("AGI:", 0, 0, textType);
  COIStringConfineToSprite(overlay->agiLabel, overlay->statWindow);
  COIStringPositionBelowString(overlay->agiLabel, overlay->defLabel);
  COIBoardAddString(board, overlay->agiLabel);

  _makeStatStrings(overlay);
}

static COIMenu* _makeTopRightMenu(COIBoard* board) {
  COISprite* frame = COISpriteCreateFromAssetID(320, 40, 250, 250,
						COI_GLOBAL_LOADER,
						5,
						COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  frame->_autoHandle = false;
  COIBoardAddDynamicSprite(board, frame);
  COISprite* pointer = COISpriteCreateFromAssetID(0, 0, 32, 32,
						  COI_GLOBAL_LOADER,
						  6,
						  COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  pointer->_autoHandle = false;
  COIBoardAddDynamicSprite(board, pointer);

  return COIMenuCreateWithCapacity(frame, pointer, BACKPACK_SIZE);
}

static void _makeBaseMenu(PauseOverlay* overlay, COITextType* textType, COIBoard* board) {
  overlay->baseMenu = _makeTopRightMenu(board);
  char* baseMenuOptions[] = { "Items", "Weapons", "Armor", "Quit Game" };
  COIString* strings[4];
  for (int i = 0; i < 4; i++) {
    char* option = baseMenuOptions[i];
    strings[i] = COIStringCreate(option, 0, 0, textType);
    COIBoardAddString(board, strings[i]);
  }
  COIMenuSetTexts(overlay->baseMenu, strings, 4);
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
    COIBoardAddString(board, string);

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
    COIBoardAddString(board, string);
    COIMenuAddString(overlay->armorMenu, string, PAUSE_OVERLAY_HEAD);
  }
  if (inventory->body->id != ITEM_ID_UNARMORED_BODY) {
    snprintf(temp, MAX_STRING_SIZE, "%s (e)", ItemListStringFromItemID(inventory->body->id));
    COIString* string = COIStringCreate(temp, 0, 0, textType);
    COIBoardAddString(board, string);
    COIMenuAddString(overlay->armorMenu, string, PAUSE_OVERLAY_BODY);
  }
  if (inventory->legs->id != ITEM_ID_UNARMORED_LEGS) {
    snprintf(temp, MAX_STRING_SIZE, "%s (e)", ItemListStringFromItemID(inventory->legs->id));
    COIString* string = COIStringCreate(temp, 0, 0, textType);    
    COIBoardAddString(board, string);
    COIMenuAddString(overlay->armorMenu, string, PAUSE_OVERLAY_LEGS);
  }
  if (inventory->weapon->id != ITEM_ID_UNARMED) {
    snprintf(temp, MAX_STRING_SIZE, "%s (e)", ItemListStringFromItemID(inventory->weapon->id));
    COIString* string = COIStringCreate(temp, 0, 0, textType);        
    COIBoardAddString(board, string);
    COIMenuAddString(overlay->weaponsMenu, string, PAUSE_OVERLAY_WEAPON);    
  }
  if (inventory->offHand->id != ITEM_ID_UNARMED_OFF) {
    snprintf(temp, MAX_STRING_SIZE, "%s (e)", ItemListStringFromItemID(inventory->offHand->id));
    COIString* string = COIStringCreate(temp, 0, 0, textType);    
    COIBoardAddString(board, string);
    COIMenuAddString(overlay->weaponsMenu, string, PAUSE_OVERLAY_OFFHAND);    
  }  
}

static void _makeConfirmMenus(PauseOverlay* overlay) {
  COIString* primary;
  COIString* cancel;

  overlay->useItemMenu = _makeTopRightMenu(overlay->board);
  primary = COIStringCreate("Use\0", 0, 0, overlay->textType);
  COIBoardAddString(overlay->board, primary);
  COIMenuAddString(overlay->useItemMenu, primary, 0);
  cancel = COIStringCreate("Cancel\0", 0, 0, overlay->textType);
  COIBoardAddString(overlay->board, cancel);
  COIMenuAddString(overlay->useItemMenu, cancel, 1);

  overlay->equipMenu = _makeTopRightMenu(overlay->board);
  primary = COIStringCreate("Equip\0", 0, 0, overlay->textType);
  COIBoardAddString(overlay->board, primary);
  COIMenuAddString(overlay->equipMenu, primary, 0);
  cancel = COIStringCreate("Cancel\0", 0, 0, overlay->textType);
  COIBoardAddString(overlay->board, cancel);
  COIMenuAddString(overlay->equipMenu, cancel, 1);

  overlay->unequipMenu = _makeTopRightMenu(overlay->board);
  primary = COIStringCreate("Unequip\0", 0, 0, overlay->textType);
  COIBoardAddString(overlay->board, primary);
  COIMenuAddString(overlay->unequipMenu, primary, 0);
  cancel = COIStringCreate("Cancel\0", 0, 0, overlay->textType);
  COIBoardAddString(overlay->board, cancel);
  COIMenuAddString(overlay->unequipMenu, cancel, 1);
}


static void _makeGearWindow(PauseOverlay* overlay, PlayerInfo* pInfo, COITextType* textType, COIBoard* board) {
  overlay->gearWindow = COISpriteCreateFromAssetID(70, 290, 500, 150,
						   COI_GLOBAL_LOADER,
						   5,
						   COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  overlay->gearWindow->_autoHandle = true;
  COIBoardAddDynamicSprite(board, overlay->gearWindow);


  Inventory* inventory = pInfo->inventory;
  char temp[MAX_STRING_SIZE];

  overlay->weaponLabel = COIStringCreate("WEAPON:", 0, 0, textType);
  COIStringConfineToSprite(overlay->weaponLabel, overlay->gearWindow);
  COIBoardAddString(board, overlay->weaponLabel);
  overlay->weapon = COIStringCreate(ItemListStringFromItemID(inventory->weapon->id), 0, 0, textType);
  COIStringConfineToSprite(overlay->weapon, overlay->gearWindow);
  COIStringPositionRightOfString(overlay->weapon, overlay->weaponLabel, 50);
  COIBoardAddString(board, overlay->weapon);
  
  overlay->offHandLabel = COIStringCreate("OFF-HAND:", 0, 0, textType);
  COIStringConfineToSprite(overlay->offHandLabel, overlay->gearWindow);
  COIStringPositionBelowString(overlay->offHandLabel, overlay->weaponLabel);
  COIBoardAddString(board, overlay->offHandLabel);
  overlay->offHand = COIStringCreate(ItemListStringFromItemID(inventory->offHand->id), 0, 0, textType);
  COIStringPositionBelowString(overlay->offHand, overlay->weapon);
  COIBoardAddString(board, overlay->offHand);

  overlay->headLabel = COIStringCreate("HEAD:", 0, 0, textType);
  COIStringConfineToSprite(overlay->headLabel, overlay->gearWindow);
  COIStringPositionBelowString(overlay->headLabel, overlay->offHandLabel);
  COIBoardAddString(board, overlay->headLabel);
  overlay->head = COIStringCreate(ItemListStringFromItemID(inventory->head->id), 0, 0, textType);
  COIStringPositionBelowString(overlay->head, overlay->offHand);
  COIBoardAddString(board, overlay->head);

  overlay->bodyLabel = COIStringCreate("BODY:", 0, 0, textType);
  COIStringConfineToSprite(overlay->bodyLabel, overlay->gearWindow);
  COIStringPositionBelowString(overlay->bodyLabel, overlay->headLabel);
  COIBoardAddString(board, overlay->bodyLabel);
  overlay->body = COIStringCreate(ItemListStringFromItemID(inventory->body->id), 0, 0, textType);
  COIStringPositionBelowString(overlay->body, overlay->head);
  COIBoardAddString(board, overlay->body);

  overlay->legsLabel = COIStringCreate("LEGS:", 0, 0, textType);
  COIStringConfineToSprite(overlay->legsLabel, overlay->gearWindow);
  COIStringPositionBelowString(overlay->legsLabel, overlay->bodyLabel);
  COIBoardAddString(board, overlay->legsLabel);
  overlay->legs = COIStringCreate(ItemListStringFromItemID(inventory->legs->id), 0, 0, textType);
  COIStringPositionBelowString(overlay->legs, overlay->body);
  COIBoardAddString(board, overlay->legs);
}

static void _destroyStatStrings(PauseOverlay* overlay) {
  COIBoardRemoveString(overlay->board, overlay->atk);
  COIStringDestroy(overlay->atk);
  overlay->atk = NULL;
  COIBoardRemoveString(overlay->board, overlay->def);
  COIStringDestroy(overlay->def);
  overlay->def = NULL;
  COIBoardRemoveString(overlay->board, overlay->agi);
  COIStringDestroy(overlay->agi);
  overlay->agi = NULL;
  COIBoardRemoveString(overlay->board, overlay->hp);
  COIStringDestroy(overlay->hp);
  overlay->hp = NULL;
  COIBoardRemoveString(overlay->board, overlay->sp);
  COIStringDestroy(overlay->sp);
  overlay->sp = NULL;
  COIBoardRemoveString(overlay->board, overlay->tp);
  COIStringDestroy(overlay->tp);
  overlay->tp = NULL;
}

static void _updateGearString(PauseOverlay* overlay, int slot) {
  COIString* oldString;
  Inventory* inventory = overlay->pInfo->inventory;
  
  switch (slot) {
  case ITEM_SLOT_WEAPON:
    oldString = overlay->weapon;
    overlay->weapon = COIStringCreate(ItemListStringFromItemID(inventory->weapon->id), oldString->x, oldString->y, overlay->textType);
    COIBoardAddString(overlay->board, overlay->weapon);
    break;
  case ITEM_SLOT_OFFHAND:
    oldString = overlay->offHand;
    overlay->offHand = COIStringCreate(ItemListStringFromItemID(inventory->offHand->id), oldString->x, oldString->y, overlay->textType);
    COIBoardAddString(overlay->board, overlay->offHand);
    break;
  case ITEM_SLOT_HEAD:
    oldString = overlay->head;
    overlay->head = COIStringCreate(ItemListStringFromItemID(inventory->head->id), oldString->x, oldString->y, overlay->textType);
    COIBoardAddString(overlay->board, overlay->head);
    break;
  case ITEM_SLOT_BODY:
    oldString = overlay->body;
    overlay->body = COIStringCreate(ItemListStringFromItemID(inventory->body->id), oldString->x, oldString->y, overlay->textType);
    COIBoardAddString(overlay->board, overlay->body);
    break;
  case ITEM_SLOT_LEGS:
    oldString = overlay->legs;
    overlay->legs = COIStringCreate(ItemListStringFromItemID(inventory->legs->id), oldString->x, oldString->y, overlay->textType);
    COIBoardAddString(overlay->board, overlay->legs);
    break;
  default:
    printf("Error: invalid slot type in pause overlay.\n");
  }
  
  COIBoardRemoveString(overlay->board, oldString);
  COIStringDestroy(oldString);
}

static void _baseMenuSelect(PauseOverlay* overlay) {
  COIMenuSetInvisible(overlay->baseMenu);
  switch (overlay->topRightMenu->_current) {
  case PAUSE_OVERLAY_ITEMS:
    overlay->topRightMenu = overlay->itemsMenu;
    break;
  case PAUSE_OVERLAY_WEAPONS:
    overlay->topRightMenu = overlay->weaponsMenu;
    _updateStatChanges(overlay, COIMenuGetCurrentValue(overlay->topRightMenu));
    break;
  case PAUSE_OVERLAY_ARMOR:
    overlay->topRightMenu = overlay->armorMenu;
    _updateStatChanges(overlay, COIMenuGetCurrentValue(overlay->topRightMenu));
    break;
  default:
    overlay->topRightMenu = overlay->baseMenu;
  }
  COIMenuSetVisible(overlay->topRightMenu);
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

PauseOverlay* PauseOverlaySelect(PauseOverlay* overlay) {
  if (overlay->topRightMenu == overlay->baseMenu) {
    _baseMenuSelect(overlay);
  } else if (overlay->topRightMenu == overlay->itemsMenu ||
	     overlay->topRightMenu == overlay->weaponsMenu ||
	     overlay->topRightMenu == overlay->armorMenu) {
    _subMenuSelect(overlay);
  } else {
    printf("invalid option\n");
  }
}

static void _destroySubMenus(PauseOverlay* overlay) {
  COIMenuDestroyAndFreeComponents(overlay->itemsMenu, overlay->board);
  COIMenuDestroyAndFreeComponents(overlay->weaponsMenu, overlay->board);
  COIMenuDestroyAndFreeComponents(overlay->armorMenu, overlay->board);
}

static void _destroyConfirmMenus(PauseOverlay* overlay) {
  COIMenuDestroyAndFreeComponents(overlay->useItemMenu, overlay->board);
  COIMenuDestroyAndFreeComponents(overlay->equipMenu, overlay->board);
  COIMenuDestroyAndFreeComponents(overlay->unequipMenu, overlay->board);
}


void PauseOverlayDestroy(PauseOverlay* overlay, COIBoard* board) {
  // Stat Window
  COIBoardRemoveString(board, overlay->name);
  COIStringDestroy(overlay->name);
  COIBoardRemoveString(board, overlay->class);
  COIStringDestroy(overlay->class);
  COIBoardRemoveString(board, overlay->hpLabel);
  COIStringDestroy(overlay->hpLabel);
  COIBoardRemoveString(board, overlay->spLabel);
  COIStringDestroy(overlay->spLabel);
  COIBoardRemoveString(board, overlay->tpLabel);
  COIStringDestroy(overlay->tpLabel);
  COIBoardRemoveString(board, overlay->atkLabel);
  COIStringDestroy(overlay->atkLabel);
  COIBoardRemoveString(board, overlay->agiLabel);
  COIStringDestroy(overlay->agiLabel);
  COIBoardRemoveString(board, overlay->defLabel);
  COIStringDestroy(overlay->defLabel);
  _destroyStatStrings(overlay);
  COIBoardRemoveDynamicSprite(board, overlay->statWindow);
  COISpriteDestroy(overlay->statWindow);

  COIBoardRemoveString(board, overlay->headLabel);
  COIStringDestroy(overlay->headLabel);
  COIBoardRemoveString(board, overlay->head);
  COIStringDestroy(overlay->head);
  COIBoardRemoveString(board, overlay->bodyLabel);
  COIStringDestroy(overlay->bodyLabel);
  COIBoardRemoveString(board, overlay->body);
  COIStringDestroy(overlay->body);
  COIBoardRemoveString(board, overlay->legsLabel);
  COIStringDestroy(overlay->legsLabel);
  COIBoardRemoveString(board, overlay->legs);
  COIStringDestroy(overlay->legs);
  COIBoardRemoveString(board, overlay->weaponLabel);
  COIStringDestroy(overlay->weaponLabel);
  COIBoardRemoveString(board, overlay->weapon);
  COIStringDestroy(overlay->weapon);
  COIBoardRemoveString(board, overlay->offHandLabel);
  COIStringDestroy(overlay->offHandLabel);
  COIBoardRemoveString(board, overlay->offHand);
  COIStringDestroy(overlay->offHand);  
  COIBoardRemoveDynamicSprite(board, overlay->gearWindow);
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
  }

  // Secondary menus always start out invisible
  COIMenuSetInvisible(overlay->itemsMenu);
  COIMenuSetInvisible(overlay->weaponsMenu);
  COIMenuSetInvisible(overlay->armorMenu);
  COIMenuSetInvisible(overlay->useItemMenu);
  COIMenuSetInvisible(overlay->equipMenu);
  COIMenuSetInvisible(overlay->unequipMenu);

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


void PauseOverlayProcessInput(PauseOverlay* overlay, int event) {
  int valueBefore = COIMenuGetCurrentValue(overlay->topRightMenu);
  COIMenuHandleInput(overlay->topRightMenu, event);
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
