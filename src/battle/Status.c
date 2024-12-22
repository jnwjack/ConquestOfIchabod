#include "Status.h"
#include "BattleBehavior.h"

void _cleanupString(COIBoard* board, COIString* string) {
  if (string) {
    COIBoardRemoveString(board, string, 0);
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
  status->cursedIcon = COISpriteCreateFromAssetID(0, 0, 32, 32, COI_GLOBAL_LOADER, 78, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  status->cursedIcon->_visible = false;
  status->cursedIcon->_autoHandle = false;
  COIBoardAddDynamicSprite(status->board, status->cursedIcon, 0);
  status->silencedIcon = COISpriteCreateFromAssetID(0, 0, 32, 32, COI_GLOBAL_LOADER, 77, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  status->silencedIcon->_visible = false;
  status->silencedIcon->_autoHandle = false;
  COIBoardAddDynamicSprite(status->board, status->silencedIcon, 0);
  status->_hpVal = -1;
  status->_tpVal = -1;
  status->_spVal = -1;
  return status;
}

// Create COIStrings based off of actor's current stats
void AllyStatusUpdate(AllyStatus* status, Actor* actor, LinkedList* modifiers) {
  COISprite* sprite = actor->sprite;
  int fs = status->textType->fontSize;

  bool shouldUpdateStats = actor->hp != status->_hpVal || actor->tp != status->_tpVal || actor->sp != status->_spVal;
  
  if (!status->frame) {
    const int sideLength = 65;
    status->frame = COISpriteCreateFromAssetID(sprite->_x + sprite->_width + 20,
					       sprite->_y - 10,
					       sideLength + 20,
					       sideLength + 20,
					       COI_GLOBAL_LOADER,
					       5,
					       COIWindowGetRenderer(COI_GLOBAL_WINDOW));
    status->frame->_autoHandle = false;
    status->frame->_visible = true;
    COIBoardAddDynamicSprite(status->board, status->frame, 0);
  }
  
  if (shouldUpdateStats) {
    _cleanupString(status->board, status->hp);
    
    char hpRaw[MAX_STRING_SIZE];
    sprintf(hpRaw, "%i/%i", actor->hp, actor->hpMax);
    COITextTypeSetColor(status->textType, 180, 0, 0);
    status->hp = COIStringCreate(hpRaw,
				 sprite->_x + sprite->_width + 30,
				 sprite->_y,
				 status->textType);
    COIStringSetVisible(status->hp, true);
    COIStringSetStringCenter(status->hp, status->frame->_x + status->frame->_width / 2, status->frame->_y + fs + COI_PADDING);
    COIBoardAddString(status->board, status->hp, 0);

    status->_hpVal = actor->hp;

    _cleanupString(status->board, status->tp);

    char tpRaw[MAX_STRING_SIZE];
    sprintf(tpRaw, "%i/%i", actor->tp, actor->tpMax);
    COITextTypeSetColor(status->textType, 0, 180, 0);
    status->tp = COIStringCreate(tpRaw,
				 sprite->_x + sprite->_width + 30,
				 sprite->_y + fs + COI_PADDING,
				 status->textType);
    COIStringSetVisible(status->tp, true);
    COIStringPositionBelowString(status->tp, status->hp, false);
    COIBoardAddString(status->board, status->tp, 0);    

    status->_tpVal = actor->tp;

    _cleanupString(status->board, status->sp);
    
    char spRaw[MAX_STRING_SIZE];
    sprintf(spRaw, "%i/%i", actor->sp, actor->spMax);
    COITextTypeSetColor(status->textType, 5, 246, 250);
    status->sp = COIStringCreate(spRaw,
          sprite->_x + sprite->_width + 30,
          sprite->_y + ((fs + COI_PADDING) * 2),
          status->textType);
    COIStringSetVisible(status->sp, true);
    COIStringPositionBelowString(status->sp, status->tp, false);
    COIBoardAddString(status->board, status->sp, 0);

    status->_spVal = actor->sp;
  }

  if (battleBehaviorCheckForModifiers(actor, MT_CURSED, modifiers)) {
    printf("found modifier\n");
    status->cursedIcon->_visible = true;
    COISpriteSetPos(status->cursedIcon, status->frame->_x, status->frame->_y + status->frame->_height + 10);
  } else {
    status->cursedIcon->_visible = false;
  }
  if (battleBehaviorCheckForModifiers(actor, MT_SILENCED, modifiers)) {
    printf("found modifier\n");
    status->silencedIcon->_visible = true;
    COISpriteSetPos(status->silencedIcon, status->frame->_x + 64, status->frame->_y + status->frame->_height + 10);
  } else {
    status->silencedIcon->_visible = false;
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
    // COIBoardRemoveDynamicSprite(status->board, status->frame);
    // COISpriteDestroy(status->frame);
  }
  
  free(status);
}
