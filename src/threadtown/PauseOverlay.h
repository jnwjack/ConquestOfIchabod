#ifndef PAUSEOVERLAY_H
#define PAUSEOVERLAY_H

#include "../player.h"
#include "../engine/COISprite.h"
#include "../engine/COIString.h"
#include "../engine/COIBoard.h"

typedef struct PauseOverlay {
  COISprite* box;
  COISprite* statWindow;
  COIString* name;
  COIString* class;

  COIString* hpLabel;
  COIString* spLabel;
  COIString* tpLabel;
  COIString* atkLabel;
  COIString* agiLabel;
  COIString* defLabel;
  bool visible;
} PauseOverlay;

PauseOverlay* PauseOverlayCreate(PlayerInfo* pInfo, COITextType* textType, COIBoard* board);
void PauseOverlayDestroy(PauseOverlay* overlay, COIBoard* board);

void PauseOverlaySetVisible(PauseOverlay* overlay, bool visible);

#endif
