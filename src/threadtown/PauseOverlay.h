#ifndef PAUSEOVERLAY_H
#define PAUSEOVERLAY_H

#include "../player.h"
#include "../engine/COISprite.h"
#include "../engine/COIString.h"
#include "../engine/COIBoard.h"
#include "../engine/COIMenu.h"

typedef struct PauseOverlay {
  COISprite* box;
  // Top left
  COISprite* statWindow;
  COIString* name;
  COIString* class;
  COIString* hpLabel;
  COIString* spLabel;
  COIString* tpLabel;
  COIString* atkLabel;
  COIString* agiLabel;
  COIString* defLabel;

  // Top right
  COIMenu* baseMenu;
  COIMenu* currentMenu;

  // Bottom
  COISprite* gearWindow;
  COIString* headLabel;
  COIString* bodyLabel;
  COIString* legsLabel;
  COIString* weaponLabel;
  COIString* offHandLabel;
  
  bool visible;
} PauseOverlay;

PauseOverlay* PauseOverlayCreate(PlayerInfo* pInfo, COITextType* textType, COIBoard* board);
void PauseOverlayDestroy(PauseOverlay* overlay, COIBoard* board);

void PauseOverlaySetVisible(PauseOverlay* overlay, bool visible);
void PauseOverlayProcessInput(PauseOverlay* overlay, int event);

#endif
