#ifndef PAUSEOVERLAY_H
#define PAUSEOVERLAY_H

#include "../player.h"
#include "../engine/COISprite.h"
#include "../engine/COIString.h"
#include "../engine/COIBoard.h"
#include "../engine/COIMenu.h"

#define PAUSE_OVERLAY_ITEMS   0
#define PAUSE_OVERLAY_WEAPONS 1
#define PAUSE_OVERLAY_ARMOR   2
#define PAUSE_OVERLAY_QUIT    3

typedef struct PauseOverlay {
  COISprite* box;
  // Top left
  COISprite* statWindow;
  COIString* name;
  COIString* class;
  COIString* hpLabel;
  COIString* hp;
  COIString* spLabel;
  COIString* sp;
  COIString* tpLabel;
  COIString* tp;
  COIString* atkLabel;
  COIString* atk;
  COIString* agiLabel;
  COIString* agi;
  COIString* defLabel;
  COIString* def;

  // Top right
  COIMenu* baseMenu;
  COIMenu* itemsMenu;
  COIMenu* weaponsMenu;
  COIMenu* armorMenu;
  COIMenu* currentMenu;

  // Bottom
  COISprite* gearWindow;
  COIString* headLabel;
  COIString* head;
  COIString* bodyLabel;
  COIString* body;
  COIString* legsLabel;
  COIString* legs;
  COIString* weaponLabel;
  COIString* weapon;
  COIString* offHandLabel;
  COIString* offHand;
  
  bool visible;
} PauseOverlay;

PauseOverlay* PauseOverlayCreate(PlayerInfo* pInfo, COITextType* textType, COIBoard* board);
void PauseOverlayDestroy(PauseOverlay* overlay, COIBoard* board);

void PauseOverlaySetVisible(PauseOverlay* overlay, bool visible);
void PauseOverlayProcessInput(PauseOverlay* overlay, int event);
PauseOverlay* PauseOverlaySelect(PauseOverlay* overlay);

#endif
