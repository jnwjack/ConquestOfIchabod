#include "Status.h"

void _cleanupString(COIBoard* board, COIString* string) {
  if (string) {
    COIBoardRemoveString(board, string);
    COIStringDestroy(string);
  }
}

AllyStatus* AllyStatusCreate(COIBoard* board, COIWindow* window, int fontSize) {
  AllyStatus* status = malloc(sizeof(AllyStatus));
  status->board = board;
  status->textType = COITextTypeCreate(fontSize, 180, 0, 0, COIWindowGetRenderer(window));
  status->frame = NULL;
  status->hp = NULL;
  status->tp = NULL;
  status->sp = NULL;
  status->_hpVal = -1;
  status->_tpVal = -1;
  status->_spVal = -1;
  return status;
}

// Create COIStrings based off of actor's current stats
void AllyStatusUpdate(AllyStatus* status, Actor* actor) {
  COISprite* sprite = actor->sprite;
  int fs = status->textType->fontSize;
  
  if (!status->frame) {
    const int sideLength = status->textType->fontSize * 5;
    status->frame = COISpriteCreateFromAssetID(sprite->_x + sprite->_width + 40,
					       sprite->_y - 10,
					       sideLength + 20,
					       sideLength + 20,
					       COI_GLOBAL_LOADER,
					       5,
					       COIWindowGetRenderer(COI_GLOBAL_WINDOW));
    status->frame->_autoHandle = false;
    status->frame->_visible = true;
    COIBoardAddDynamicSprite(status->board, status->frame);
  }
  
  if (actor->hp != status->_hpVal) {
    _cleanupString(status->board, status->hp);
    
    char hpRaw[MAX_STRING_SIZE];
    sprintf(hpRaw, "%i/%i", actor->hp, actor->hpMax);
    COITextTypeSetColor(status->textType, 180, 0, 0);
    status->hp = COIStringCreate(hpRaw,
				 sprite->_x + sprite->_width + 50,
				 sprite->_y,
				 status->textType);
    COIStringSetVisible(status->hp, true);
    COIBoardAddString(status->board, status->hp);

    status->_hpVal = actor->hp;
  }
  if (actor->tp != status->_tpVal) {
    _cleanupString(status->board, status->tp);

    char tpRaw[MAX_STRING_SIZE];
    sprintf(tpRaw, "%i/%i", actor->tp, actor->tpMax);
    COITextTypeSetColor(status->textType, 0, 180, 0);
    status->tp = COIStringCreate(tpRaw,
				 sprite->_x + sprite->_width + 50,
				 sprite->_y + fs + COI_PADDING,
				 status->textType);
    COIStringSetVisible(status->tp, true);
    COIBoardAddString(status->board, status->tp);    

    status->_tpVal = actor->tp;
  }
  if (actor->sp != status->_spVal) {
    _cleanupString(status->board, status->sp);
    
    char spRaw[MAX_STRING_SIZE];
    sprintf(spRaw, "%i/%i", actor->sp, actor->spMax);
    COITextTypeSetColor(status->textType, 5, 246, 250);
    status->sp = COIStringCreate(spRaw,
          sprite->_x + sprite->_width + 50,
          sprite->_y + ((fs + COI_PADDING) * 2),
          status->textType);
    COIStringSetVisible(status->sp, true);
    COIBoardAddString(status->board, status->sp);

    status->_spVal = actor->sp;
  }
}

void AllyStatusSetVisible(AllyStatus* status, bool visible) {
  status->frame->_visible = false;
  COIStringSetVisible(status->hp, false);
  COIStringSetVisible(status->sp, false);
  COIStringSetVisible(status->tp, false);
}

void AllyStatusDestroy(AllyStatus* status) {
  COITextTypeDestroy(status->textType);
  _cleanupString(status->board, status->hp);
  _cleanupString(status->board, status->tp);
  _cleanupString(status->board, status->sp);
  if (status->frame != NULL) {
    COIBoardRemoveDynamicSprite(status->board, status->frame);
    COISpriteDestroy(status->frame);
  }
  
  free(status);
}
