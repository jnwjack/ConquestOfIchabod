#include "PauseOverlay.h"

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
  snprintf(temp, MAX_STRING_SIZE, "%i / %i", player->hp, player->hpMax);
  overlay->hp = COIStringCreate(temp, 0, 0, textType);
  COIStringPositionBelowString(overlay->hp, overlay->class);
  COIStringPositionRightOfString(overlay->hp, overlay->hpLabel, 50);
  COIBoardAddString(board, overlay->hp);

  overlay->spLabel = COIStringCreate("SP:", 0, 0, textType);
  COIStringConfineToSprite(overlay->spLabel, overlay->statWindow);
  COIStringPositionBelowString(overlay->spLabel, overlay->hpLabel);
  COIBoardAddString(board, overlay->spLabel);
  snprintf(temp, MAX_STRING_SIZE, "%i / %i", player->sp, player->spMax);
  overlay->sp = COIStringCreate(temp, 0, 0, textType);
  COIStringPositionBelowString(overlay->sp, overlay->hp);
  COIBoardAddString(board, overlay->sp);

  overlay->tpLabel = COIStringCreate("TP:", 0, 0, textType);
  COIStringConfineToSprite(overlay->tpLabel, overlay->statWindow);
  COIStringPositionBelowString(overlay->tpLabel, overlay->spLabel);
  COIBoardAddString(board, overlay->tpLabel);
  snprintf(temp, MAX_STRING_SIZE, "%i / %i", player->tp, player->tpMax);
  overlay->tp = COIStringCreate(temp, 0, 0, textType);
  COIStringPositionBelowString(overlay->tp, overlay->sp);
  COIBoardAddString(board, overlay->tp);

  overlay->atkLabel = COIStringCreate("ATK:", 0, 0, textType);
  COIStringConfineToSprite(overlay->atkLabel, overlay->statWindow);
  COIStringPositionBelowString(overlay->atkLabel, overlay->tpLabel);
  COIBoardAddString(board, overlay->atkLabel);
  snprintf(temp, MAX_STRING_SIZE, "%i", player->atk);
  overlay->atk = COIStringCreate(temp, 0, 0, textType);
  COIStringPositionBelowString(overlay->atk, overlay->tp);
  COIBoardAddString(board, overlay->atk);
  
  overlay->defLabel = COIStringCreate("DEF:", 0, 0, textType);
  COIStringConfineToSprite(overlay->defLabel, overlay->statWindow);
  COIStringPositionBelowString(overlay->defLabel, overlay->atkLabel);
  COIBoardAddString(board, overlay->defLabel);
  snprintf(temp, MAX_STRING_SIZE, "%i", player->def);
  overlay->def = COIStringCreate(temp, 0, 0, textType);
  COIStringPositionBelowString(overlay->def, overlay->atk);
  COIBoardAddString(board, overlay->def);
  
  overlay->agiLabel = COIStringCreate("AGI:", 0, 0, textType);
  COIStringConfineToSprite(overlay->agiLabel, overlay->statWindow);
  COIStringPositionBelowString(overlay->agiLabel, overlay->defLabel);
  COIBoardAddString(board, overlay->agiLabel);
  snprintf(temp, MAX_STRING_SIZE, "%i", player->agi);
  overlay->agi = COIStringCreate(temp, 0, 0, textType);
  COIStringPositionBelowString(overlay->agi, overlay->def);
  COIBoardAddString(board, overlay->agi);
}

static COIMenu* _makeMenu(COIBoard* board) {
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
  overlay->baseMenu = _makeMenu(board);
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
  overlay->itemsMenu = _makeMenu(board);
  overlay->weaponsMenu = _makeMenu(board);
  overlay->armorMenu = _makeMenu(board);
  Inventory* inventory = pInfo->inventory;
  
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

static void _baseMenuSelect(PauseOverlay* overlay) {
  COIMenuSetInvisible(overlay->baseMenu);
  switch (overlay->currentMenu->_current) {
  case PAUSE_OVERLAY_ITEMS:
    overlay->currentMenu = overlay->itemsMenu;
    break;
  case PAUSE_OVERLAY_WEAPONS:
    overlay->currentMenu = overlay->weaponsMenu;
    break;
  case PAUSE_OVERLAY_ARMOR:
    overlay->currentMenu = overlay->armorMenu;
    break;
  default:
    overlay->currentMenu = overlay->baseMenu;
  }
  COIMenuSetVisible(overlay->currentMenu);
}

PauseOverlay* PauseOverlayCreate(PlayerInfo* pInfo, COITextType* textType, COIBoard* board) {
  // May want to have several separate boxes, not just the single big one
  
  PauseOverlay* overlay = malloc(sizeof(PauseOverlay));

  _makeStatWindow(overlay, pInfo, textType, board);
  _makeBaseMenu(overlay, textType, board);
  _makeItemsMenu(overlay, pInfo, textType, board);
  _makeGearWindow(overlay, pInfo, textType, board);

  overlay->currentMenu = overlay->baseMenu;

  PauseOverlaySetVisible(overlay, false);
  return overlay;
}

PauseOverlay* PauseOverlaySelect(PauseOverlay* overlay) {
  if (overlay->currentMenu == overlay->baseMenu) {
    _baseMenuSelect(overlay);
  } else {
    printf("invalid option\n");
  }
}

void PauseOverlayDestroy(PauseOverlay* overlay, COIBoard* board) {
  // Stat Window
  COIBoardRemoveString(board, overlay->name);
  COIStringDestroy(overlay->name);
  COIBoardRemoveString(board, overlay->class);
  COIStringDestroy(overlay->class);
  COIBoardRemoveString(board, overlay->hpLabel);
  COIStringDestroy(overlay->hpLabel);
  COIBoardRemoveString(board, overlay->hp);
  COIStringDestroy(overlay->hp);
  COIBoardRemoveString(board, overlay->spLabel);
  COIStringDestroy(overlay->spLabel);
  COIBoardRemoveString(board, overlay->sp);
  COIStringDestroy(overlay->sp);
  COIBoardRemoveString(board, overlay->tpLabel);
  COIStringDestroy(overlay->tpLabel);
  COIBoardRemoveString(board, overlay->tp);
  COIStringDestroy(overlay->tp);  
  COIBoardRemoveString(board, overlay->atkLabel);
  COIStringDestroy(overlay->atkLabel);
  COIBoardRemoveString(board, overlay->atk);
  COIStringDestroy(overlay->atk);
  COIBoardRemoveString(board, overlay->agiLabel);
  COIStringDestroy(overlay->agiLabel);
  COIBoardRemoveString(board, overlay->agi);
  COIStringDestroy(overlay->agi);
  COIBoardRemoveString(board, overlay->defLabel);
  COIStringDestroy(overlay->defLabel);
  COIBoardRemoveString(board, overlay->def);
  COIStringDestroy(overlay->def);
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
  COIMenuDestroyAndFreeComponents(overlay->itemsMenu, board);
  COIMenuDestroyAndFreeComponents(overlay->weaponsMenu, board);
  COIMenuDestroyAndFreeComponents(overlay->armorMenu, board);

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

  // Secondary menus always start out invisible
  COIMenuSetInvisible(overlay->itemsMenu);
  COIMenuSetInvisible(overlay->weaponsMenu);
  COIMenuSetInvisible(overlay->armorMenu);

  overlay->visible = visible;
  overlay->currentMenu = overlay->baseMenu;
}

void PauseOverlayProcessInput(PauseOverlay* overlay, int event) {
  COIMenuHandleInput(overlay->currentMenu, event);
}
