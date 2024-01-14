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
  
  snprintf(temp, MAX_STRING_SIZE, "HP: %i / %i", player->hp, player->hpMax);
  overlay->hpLabel = COIStringCreate(temp, 0, 0, textType);
  COIStringConfineToSprite(overlay->hpLabel, overlay->statWindow);
  COIStringPositionBelowString(overlay->hpLabel, overlay->class);
  COIBoardAddString(board, overlay->hpLabel);

  snprintf(temp, MAX_STRING_SIZE, "SP: %i / %i", player->sp, player->spMax);
  overlay->spLabel = COIStringCreate(temp, 0, 0, textType);
  COIStringConfineToSprite(overlay->spLabel, overlay->statWindow);
  COIStringPositionBelowString(overlay->spLabel, overlay->hpLabel);
  COIBoardAddString(board, overlay->spLabel);

  snprintf(temp, MAX_STRING_SIZE, "TP: %i / %i", player->tp, player->tpMax);
  overlay->tpLabel = COIStringCreate(temp, 0, 0, textType);
  COIStringConfineToSprite(overlay->tpLabel, overlay->statWindow);
  COIStringPositionBelowString(overlay->tpLabel, overlay->spLabel);
  COIBoardAddString(board, overlay->tpLabel);

  snprintf(temp, MAX_STRING_SIZE, "ATK: %i", player->atk);
  overlay->atkLabel = COIStringCreate(temp, 0, 0, textType);
  COIStringConfineToSprite(overlay->atkLabel, overlay->statWindow);
  COIStringPositionBelowString(overlay->atkLabel, overlay->tpLabel);
  COIBoardAddString(board, overlay->atkLabel);

  snprintf(temp, MAX_STRING_SIZE, "DEF: %i", player->def);
  overlay->defLabel = COIStringCreate(temp, 0, 0, textType);
  COIStringConfineToSprite(overlay->defLabel, overlay->statWindow);
  COIStringPositionBelowString(overlay->defLabel, overlay->atkLabel);
  COIBoardAddString(board, overlay->defLabel);
  
  snprintf(temp, MAX_STRING_SIZE, "AGI: %i", player->agi);
  overlay->agiLabel = COIStringCreate(temp, 0, 0, textType);
  COIStringConfineToSprite(overlay->agiLabel, overlay->statWindow);
  COIStringPositionBelowString(overlay->agiLabel, overlay->defLabel);
  COIBoardAddString(board, overlay->agiLabel);
}

static void _makeBaseMenu(PauseOverlay* overlay, COITextType* textType, COIBoard* board) {
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
  overlay->baseMenu = COIMenuCreate(frame, pointer);
  char* baseMenuOptions[] = { "Items", "Weapons", "Armor", "Quit Game" };
  COIString* strings[4];
  for (int i = 0; i < 4; i++) {
    char* option = baseMenuOptions[i];
    strings[i] = COIStringCreate(option, 0, 0, textType);
    COIBoardAddString(board, strings[i]);
  }
  COIMenuSetTexts(overlay->baseMenu, strings, 4);
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

  snprintf(temp, MAX_STRING_SIZE, "WEAPON: %s",  ItemListStringFromItemID(inventory->weapon->id));
  overlay->weaponLabel = COIStringCreate(temp, 0, 0, textType);
  COIStringConfineToSprite(overlay->weaponLabel, overlay->gearWindow);
  COIBoardAddString(board, overlay->weaponLabel);

  snprintf(temp, MAX_STRING_SIZE, "OFF-HAND: %s",  ItemListStringFromItemID(inventory->offHand->id));
  overlay->offHandLabel = COIStringCreate(temp, 0, 0, textType);
  COIStringConfineToSprite(overlay->offHandLabel, overlay->gearWindow);
  COIStringPositionBelowString(overlay->offHandLabel, overlay->weaponLabel);
  COIBoardAddString(board, overlay->offHandLabel);

  snprintf(temp, MAX_STRING_SIZE, "HEAD: %s",  ItemListStringFromItemID(inventory->head->id));
  overlay->headLabel = COIStringCreate(temp, 0, 0, textType);
  COIStringConfineToSprite(overlay->headLabel, overlay->gearWindow);
  COIStringPositionBelowString(overlay->headLabel, overlay->offHandLabel);
  COIBoardAddString(board, overlay->headLabel);

  snprintf(temp, MAX_STRING_SIZE, "BODY: %s",  ItemListStringFromItemID(inventory->body->id));
  overlay->bodyLabel = COIStringCreate(temp, 0, 0, textType);
  COIStringConfineToSprite(overlay->bodyLabel, overlay->gearWindow);
  COIStringPositionBelowString(overlay->bodyLabel, overlay->headLabel);
  COIBoardAddString(board, overlay->bodyLabel);

  snprintf(temp, MAX_STRING_SIZE, "LEGS: %s",  ItemListStringFromItemID(inventory->legs->id));
  overlay->legsLabel = COIStringCreate(temp, 0, 0, textType);
  COIStringConfineToSprite(overlay->legsLabel, overlay->gearWindow);
  COIStringPositionBelowString(overlay->legsLabel, overlay->bodyLabel);
  COIBoardAddString(board, overlay->legsLabel);
}

PauseOverlay* PauseOverlayCreate(PlayerInfo* pInfo, COITextType* textType, COIBoard* board) {
  // May want to have several separate boxes, not just the single big one
  
  PauseOverlay* overlay = malloc(sizeof(PauseOverlay));

  _makeStatWindow(overlay, pInfo, textType, board);
  _makeBaseMenu(overlay, textType, board);
  _makeGearWindow(overlay, pInfo, textType, board);

  overlay->currentMenu = overlay->baseMenu;

  PauseOverlaySetVisible(overlay, false);
  return overlay;
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
  COIBoardRemoveDynamicSprite(board, overlay->statWindow);
  COISpriteDestroy(overlay->statWindow);

  COIBoardRemoveString(board, overlay->headLabel);
  COIStringDestroy(overlay->headLabel);
  COIBoardRemoveString(board, overlay->bodyLabel);
  COIStringDestroy(overlay->bodyLabel);
  COIBoardRemoveString(board, overlay->legsLabel);
  COIStringDestroy(overlay->legsLabel);
  COIBoardRemoveString(board, overlay->weaponLabel);
  COIStringDestroy(overlay->weaponLabel);
  COIBoardRemoveString(board, overlay->offHandLabel);
  COIStringDestroy(overlay->offHandLabel);
  COIBoardRemoveDynamicSprite(board, overlay->gearWindow);
  COISpriteDestroy(overlay->gearWindow);

  COIMenuDestroyAndFreeComponents(overlay->baseMenu, board);

  free(overlay);
}

void PauseOverlaySetVisible(PauseOverlay* overlay, bool visible) {
  COIStringSetVisible(overlay->name, visible);
  COIStringSetVisible(overlay->class, visible);
  COIStringSetVisible(overlay->hpLabel, visible);
  COIStringSetVisible(overlay->spLabel, visible);
  COIStringSetVisible(overlay->tpLabel, visible);
  COIStringSetVisible(overlay->atkLabel, visible);
  COIStringSetVisible(overlay->agiLabel, visible);
  COIStringSetVisible(overlay->defLabel, visible);
  overlay->statWindow->_visible = visible;

  COIStringSetVisible(overlay->headLabel, visible);
  COIStringSetVisible(overlay->bodyLabel, visible);
  COIStringSetVisible(overlay->legsLabel, visible);
  COIStringSetVisible(overlay->weaponLabel, visible);
  COIStringSetVisible(overlay->offHandLabel, visible);
  overlay->gearWindow->_visible = visible;
  
  if (visible) {
    COIMenuSetVisible(overlay->baseMenu);
  } else {
    COIMenuSetInvisible(overlay->baseMenu);
  }

  overlay->visible = visible;
}

void PauseOverlayProcessInput(PauseOverlay* overlay, int event) {
  COIMenuHandleInput(overlay->currentMenu, event);
}
