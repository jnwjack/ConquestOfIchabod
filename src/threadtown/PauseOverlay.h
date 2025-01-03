#ifndef PAUSEOVERLAY_H
#define PAUSEOVERLAY_H

#include "../player.h"
#include "../TextBox.h"

#define PAUSE_OVERLAY_ITEMS   0
#define PAUSE_OVERLAY_WEAPONS 1
#define PAUSE_OVERLAY_ARMOR   2
#define PAUSE_OVERLAY_OPTIONS 3
#define PAUSE_OVERLAY_QUIT    4

// Used as return values for equipped items in menus
#define PAUSE_OVERLAY_WEAPON -1
#define PAUSE_OVERLAY_OFFHAND -2
#define PAUSE_OVERLAY_HEAD -3
#define PAUSE_OVERLAY_BODY -4
#define PAUSE_OVERLAY_LEGS -5

#define PAUSE_OVERLAY_LABEL_STAT_PADDING 10

typedef struct PauseOverlay {
  COISprite* box;
  // Top left
  COISprite* statWindow;
  COIString* name;
  COIString* class;
  COIString* lvLabel;
  COIString* lv;
  COIString* goldLabel;
  COIString* gold;
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

  // Displays potential change to stats when scrolling through gear
  COIString* atkChange;
  COIString* defChange;
  COIString* agiChange;
  COIString* hpChange;
  COIString* spChange;
  COIString* tpChange;
  COITextType* posTextType;
  COITextType* negTextType;

  // Top right
  COIMenu* baseMenu;
  COIMenu* itemsMenu;
  COIMenu* weaponsMenu;
  COIMenu* armorMenu;
  
  COIMenu* topRightMenu;

  // Center
  COIMenu* useItemMenu;
  COIMenu* equipMenu;
  COIMenu* unequipMenu;
  COIMenu* quitMenu;

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

  Item* selectedItem;

  COIPreferencesMenu prefMenu;
  
  bool visible;
  bool dirty; // True when we need to rebuild the menus due to changed inventory
  COITextType* textType;
  COIBoard* board;
  PlayerInfo* pInfo;
} PauseOverlay;

PauseOverlay* PauseOverlayCreate(PlayerInfo* pInfo, COITextType* textType, COIBoard* board);
void PauseOverlayDestroy(PauseOverlay* overlay, COIBoard* board);

void PauseOverlaySetVisible(PauseOverlay* overlay, bool visible);
void PauseOverlayToggleVisibility(PauseOverlay* overlay);
void PauseOverlayProcessInput(PauseOverlay* overlay, int event);
void PauseOverlaySelect(PauseOverlay* overlay, TextBox* textBox);
void PauseOverlayBack(PauseOverlay* overlay);

#endif
