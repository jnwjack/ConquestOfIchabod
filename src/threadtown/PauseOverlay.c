#include "PauseOverlay.h"

void _makeStatWindow(PauseOverlay* overlay, PlayerInfo* pInfo, COITextType* textType, COIBoard* board) {
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
  
  sprintf(temp, "HP: %i / %i", player->hp, player->hpMax);
  overlay->hpLabel = COIStringCreate(temp, 0, 0, textType);
  COIStringConfineToSprite(overlay->hpLabel, overlay->statWindow);
  COIStringPositionBelowString(overlay->hpLabel, overlay->class);
  COIBoardAddString(board, overlay->hpLabel);

  sprintf(temp, "SP: %i / %i", player->sp, player->spMax);
  overlay->spLabel = COIStringCreate(temp, 0, 0, textType);
  COIStringConfineToSprite(overlay->spLabel, overlay->statWindow);
  COIStringPositionBelowString(overlay->spLabel, overlay->hpLabel);
  COIBoardAddString(board, overlay->spLabel);

  sprintf(temp, "TP: %i / %i", player->tp, player->tpMax);
  overlay->tpLabel = COIStringCreate(temp, 0, 0, textType);
  COIStringConfineToSprite(overlay->tpLabel, overlay->statWindow);
  COIStringPositionBelowString(overlay->tpLabel, overlay->spLabel);
  COIBoardAddString(board, overlay->tpLabel);

  sprintf(temp, "ATK: %i", player->atk);
  overlay->atkLabel = COIStringCreate(temp, 0, 0, textType);
  COIStringConfineToSprite(overlay->atkLabel, overlay->statWindow);
  COIStringPositionBelowString(overlay->atkLabel, overlay->tpLabel);
  COIBoardAddString(board, overlay->atkLabel);

  sprintf(temp, "DEF: %i", player->def);
  overlay->defLabel = COIStringCreate(temp, 0, 0, textType);
  COIStringConfineToSprite(overlay->defLabel, overlay->statWindow);
  COIStringPositionBelowString(overlay->defLabel, overlay->atkLabel);
  COIBoardAddString(board, overlay->defLabel);
  
  sprintf(temp, "AGI: %i", player->agi);
  overlay->agiLabel = COIStringCreate(temp, 0, 0, textType);
  COIStringConfineToSprite(overlay->agiLabel, overlay->statWindow);
  COIStringPositionBelowString(overlay->agiLabel, overlay->defLabel);
  COIBoardAddString(board, overlay->agiLabel);
}

PauseOverlay* PauseOverlayCreate(PlayerInfo* pInfo, COITextType* textType, COIBoard* board) {
  // May want to have several separate boxes, not just the single big one
  
  PauseOverlay* overlay = malloc(sizeof(PauseOverlay));
  /*overlay->box = COISpriteCreateFromAssetID(70, 40, 500, 400,
					    COI_GLOBAL_LOADER,
					    5,
					    COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  */
  _makeStatWindow(overlay, pInfo, textType, board);

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

  overlay->visible = visible;
}
