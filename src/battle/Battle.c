#include "Battle.h"
#include "../actor.h"

COIBoard* battleCreateBoard(COIWindow* window, COIAssetLoader* loader,
			    COIBoard* outsideBoard, COILoop outsideLoop,
			    int enemyType, PlayerInfo* pInfo) {  
  COIBoard* board = COIBoardCreate(99, 91, 95, 225, 640, 480, loader);
  COIBoardLoadSpriteMap(board, COIWindowGetRenderer(window), "src/battle/spritemap.dat");

  // Context
  BattleContext* context = malloc(sizeof(BattleContext));
  context->pointingAtEnemies = false;
  context->targetedActorIndex = 0;

  // Required for determining what to do after battle ends
  context->outside = outsideBoard;
  context->outsideLoop = outsideLoop;
  context->window = window;

  // Allies
  context->allies = pInfo->party;
  context->numAllies = pInfo->partySize;

  // Enemies, can later randomize number
  context->numEnemies = 3;
  
  // Keep a list of all strings we have that we can pass to the COIBoard
  int totalStrings = BATTLE_NUM_ACTIONS + context->numEnemies + context->numAllies;
  COIString** allStrings = malloc(sizeof(COIString*) * (BATTLE_NUM_ACTIONS + context->numEnemies + context->numAllies));

  context->textType = COITextTypeCreate(25, 255, 255, 255, COIWindowGetRenderer(window));
  
  context->actionStrings[0] = COIStringCreate("Attack", 0, 0, context->textType);
  context->actionStrings[1] = COIStringCreate("Tech", 0, 0, context->textType);
  context->actionStrings[2] = COIStringCreate("Special", 0, 0, context->textType);
  context->actionStrings[3] = COIStringCreate("Item", 0, 0, context->textType);
  context->actionStrings[4] = COIStringCreate("Flee", 0, 0, context->textType);
  for (int i = 0; i < BATTLE_NUM_ACTIONS; i++) {
    allStrings[i] = context->actionStrings[i];
  }

  COISprite** sprites = COIBoardGetSprites(board);

  context->actionMenu = COIMenuCreate(sprites[3], sprites[4]);
  COIMenuSetTexts(context->actionMenu, context->actionStrings, BATTLE_NUM_ACTIONS);
  //COIMenuIncrement(context->actionMenu, 1);
  COIMenuSetVisible(context->actionMenu);
  context->actionMenuFocused = true;

  // Pointer for enemies and allies
  context->pointer = sprites[5];
  context->pointer->_autoHandle = false;
  context->pointer->_visible = false;

  int offsetX = 50, offsetY = 70;
  context->enemies = malloc(sizeof(Actor*) * context->numEnemies);
  context->enemyNames = malloc(sizeof(COIString*) * context->numEnemies);
  for (int i = 0; i < context->numEnemies; i++) {
    context->enemies[i] = actorCreateOfType(enemyType, offsetX, offsetY + 80*i, loader, window);
    char name[MAX_STRING_SIZE];
    sprintf(name, "%s %i", actorGetNameFromType(enemyType), i + 1);
    context->enemyNames[i] = COIStringCreate(name, 220, 40, context->textType);
    COIStringSetVisible(context->enemyNames[i], false);
    allStrings[BATTLE_NUM_ACTIONS+i] = context->enemyNames[i];
  }
  COIBoardSetDynamicSprites(board, actorGetSpriteList(context->enemies, context->numEnemies), context->numEnemies);

  COIBoardSetStrings(board, allStrings, BATTLE_NUM_ACTIONS + context->numEnemies);

  COIBoardSetContext(board, (void*)context);

  return board;
}

// Adjust pointer sprite coords to be pointing at targeted actor
void _adjustPointer(BattleContext* context) {
  COISprite* target =  context->enemies[context->targetedActorIndex]->sprite;
  COISprite* pointer = context->pointer;
  
  int newY = target->_y + (target->_height / 2) - pointer->_height / 2;
  int newX = target->_x - BATTLE_POINTER_PADDING_X - pointer->_width;
  COISpriteSetPos(pointer, newX, newY);
}

COISprite* _toggleTargetNameVisibility(BattleContext* context, bool visible) {
  COIString* name = context->enemyNames[context->targetedActorIndex];
  COIStringSetVisible(name, visible);
}

// Move pointer to actor 'offset' spaces away
void battleMovePointer(BattleContext* context, int offset) {
  // Make name of the previous actor invisible
  _toggleTargetNameVisibility(context, false);
  
  // Loop around if we need to
  int newTargetIndex = context->targetedActorIndex + offset;
  if (newTargetIndex < 0) {
    int offsetFromEnd = (-1 * newTargetIndex) % context->numEnemies;
    context->targetedActorIndex = context->numEnemies - offsetFromEnd;
  } else {
    context->targetedActorIndex = newTargetIndex % context->numEnemies;
  }

  _adjustPointer(context);
  // Show name of new actor
  _toggleTargetNameVisibility(context, true);
}

// Reset everything so that we're looking at the action menu again
void _focusActionMenu(BattleContext* context) {
  _toggleTargetNameVisibility(context, false);
  // Reset targeted actor back to 0
  context->targetedActorIndex = 0;
  context->pointer->_visible = false;

  context->actionMenuFocused = true;
}

void _attack(BattleContext* context) {
  context->pointingAtEnemies = true;
  context->targetedActorIndex = 0;
  _adjustPointer(context);
  _toggleTargetNameVisibility(context, true);
  context->pointer->_visible = true;
}

// When selecting what character should do, handle each option in menu.
bool battleHandleActionSelection(BattleContext* context) {
  switch (context->actionMenu->_current) {
  case BATTLE_ATTACK:
    _attack(context);
    break;
  case BATTLE_FLEE:
    // Replace this with probability check, flee may fail
    return true;
  default:
    printf("Invalid action in battle.\n");
    return false;
  }
  
  context->actionMenuFocused = false;
  return false;
}

// User presses 'LEFT' key, cancels out of current operation
void battleHandleBack(BattleContext* context) {
  switch (context->actionMenu->_current) {
  case BATTLE_ATTACK:
    _focusActionMenu(context);
    break;
  default:
    printf("Invalid action in battle.\n");
  }
}

void battleDestroyBoard(COIBoard* board) {
  BattleContext* context = (BattleContext*)board->context;
  COIMenuDestroy(context->actionMenu);
  COITextTypeDestroy(context->textType);
  for (int i = 0; i < BATTLE_NUM_ACTIONS; i++) {
    COIStringDestroy(context->actionStrings[i]);
  }
  //free(context->actionStrings);
  context->actionMenu->_strings = NULL;
  for (int i = 0; i < context->numEnemies; i++) {
    actorDestroy(context->enemies[i]);
    COIStringDestroy(context->enemyNames[i]);
  }
  free(context->enemies);
  free(context);
  COIBoardDestroy(board);
}
