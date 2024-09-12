#ifndef BATTLE_H
#define BATTLE_H

/*#include "../engine/COIBoard.h"
#include "../engine/COIWindow.h"
#include "../engine/COIMenu.h"
#include "../engine/COIString.h"
#include "../engine/COISprite.h"
*/
//#include "../player.h"
#include "BattleBehavior.h"
#include "Status.h"
#include "BattleSplash.h"
#include "../terrain.h"

#define BATTLE_ATTACK 0
#define BATTLE_TECH 1
#define BATTLE_SPECIAL 2
#define BATTLE_ITEM 3
#define BATTLE_FLEE 4

#define BATTLE_POINTER_PADDING_X 10
#define BATTLE_NUM_ACTIONS 5
#define BATTLE_MAX_MOVEMENT 30
#define BATTLE_MOVEMENT_STEP 1

// Actor positions
#define BATTLE_E_OFFSET_X 50
#define BATTLE_A_OFFSET_X 400
#define BATTLE_OFFSET_Y 50
#define BATTLE_Y_STEP 80

// Spritemap positions
#define BATTLE_SPRITEMAP_A_BOX 9
#define BATTLE_SPRITEMAP_E_BOX 8
#define BATTLE_SPRITEMAP_DESC_BOX 16
// #define BATTLE_SPRITEMAP_SPLASH_BOX 17
#define BATTLE_SPRITEMAP_POINTER 13
#define BATTLE_SPRITEMAP_MENU_FRAME 11
#define BATTLE_SPRITEMAP_MENU_POINTER 12
#define BATTLE_SPRITEMAP_SUBMENU_FRAME 14
#define BATTLE_SPRITEMAP_SUBMENU_POINTER 15
#define BATTLE_SPRITEMAP_NAME_BOX 10

#define BATTLE_TECH_PARTICLE_TICKS 4

typedef enum{
  SS_MOVE_FORWARD,
  SS_TEXT,
  SS_MOVE_BACKWARDS,
  SS_SPLASH
} BattleSceneStage;

typedef enum {
  ACTION_MENU,
  SUB_MENU,
  ACTORS,
  LEVEL_UP
} BattleMenuFocus;

typedef enum {
  SM_TECH,
  SM_SPECIAL,
  SM_ITEM
} BattleSubMenu;

typedef enum {
  BR_LOSS,
  BR_WIN,
  BR_FLEE,
  BR_CONTINUE
} BattleResult;

typedef struct BattleContext {
  COIBoard* board;
  PlayerInfo* pInfo;
  
  // Actions: Attack, Tech, Special, etc.
  COIMenu* actionMenu;
  COIString* actionStrings[BATTLE_NUM_ACTIONS];

  // Secondary menu when Item, Tech, Special actions selected
  COIMenu* subMenu;

  BattleMenuFocus menuFocus;
  BattleSubMenu subMenuType;

  COITextType* textType;
  int numStrings; // Count of COIStrings in context

  Actor** enemies;
  COIString** enemyNames;
  int numEnemies;

  Actor** allies;
  LinkedList* modifiers;
  COIString** allyNames;
  int numAllies;
  AllyStatus** allyStatuses;

  BattleAction* actions; // Queue of turn's actions, 1 per actor
  ActionSummary* summary;

  // Pointer for enemies and allies
  COISprite* pointer;
  // Is the pointer over the enemies or the allies?
  bool pointingAtEnemies;
  // Who are we looking at?
  int targetedActorIndex;

  // Which ally is acting?
  int turnIndex;

  // Battle Scenes
  bool controlEnabled; // False when playing animations/processing actions
  int currentActionIndex; // Action we're currently processing
  BattleSceneStage sceneStage; // Current stage in action processing
  int movementOffset; // How far has actor moved so far?

  // End splash screen, XP gained, new items, progress bar, level up, etc..
  BattleSplash* splash;
  LevelUpSplash* levelUpSplash;

  // TECH particle effects
  COISprite** techParticles;
  //LinkedList* techParticles;

  unsigned long xpYield;
  unsigned int gold;

  // Data for next board after battle completes
  COIBoard* outside;
  COILoop outsideLoop;
  COIWindow* window;
  int playerOutsideX;
  int playerOutsideY;
} BattleContext;

COIBoard* battleCreateBoard(COIWindow* window, COIAssetLoader* loader,
			    COIBoard* outsideBoard, COILoop outsideLoop,
			    Terrain terrain, PlayerInfo* pInfo);
void battleDestroyBoard(COIBoard* board);
void battleTick(BattleContext* context);
void battleHandleBack(BattleContext* context);
COIMenu* battleGetFocusedMenu(BattleContext* context);
BattleResult battleHandleActionSelection(BattleContext* context);
void battleHandleSubMenuSelection(BattleContext* context);
void battleHandleActorSelect(BattleContext* context);
void battleMovePointer(BattleContext* context, int direction);
void battleSelectAttackTarget(BattleContext* context);
BattleResult battleAdvanceScene(BattleContext* context, bool selection);

#endif
