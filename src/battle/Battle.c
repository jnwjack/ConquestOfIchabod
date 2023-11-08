#include "Battle.h"
#include "../actor.h"

int _getNumStrings(BattleContext* context) {
  int count = BATTLE_NUM_ACTIONS + context->numEnemies + context->numAllies;

  return count;
}

COIString** _makeStrings(BattleContext* context, PlayerInfo* pInfo, COIBoard* board) {
  context->numStrings = _getNumStrings(context);
  
  COIString** allStrings = malloc(sizeof(COIString*) * context->numStrings);
  
  context->actionStrings[0] = COIStringCreate("Attack", 0, 0, context->textType);
  context->actionStrings[1] = COIStringCreate("Tech", 0, 0, context->textType);
  context->actionStrings[2] = COIStringCreate("Special", 0, 0, context->textType);
  context->actionStrings[3] = COIStringCreate("Item", 0, 0, context->textType);
  context->actionStrings[4] = COIStringCreate("Flee", 0, 0, context->textType);
  for (int i = 0; i < BATTLE_NUM_ACTIONS; i++) {
    allStrings[i] = context->actionStrings[i];
  }
  
  context->enemyNames = malloc(sizeof(COIString*) * context->numEnemies);
  for (int i = 0; i < context->numEnemies; i++) {
    Actor* enemy = context->enemies[i];
    char name[MAX_STRING_SIZE];
    sprintf(name, "%s %i", actorGetNameFromType(enemy->actorType), i + 1);
    context->enemyNames[i] = COIStringCreate(name, 220, 40, context->textType);
    COIStringSetVisible(context->enemyNames[i], false);
    allStrings[BATTLE_NUM_ACTIONS+i] = context->enemyNames[i];
  }

  //int aOffsetX = 400, aOffsetY = 70;
  context->allyNames = malloc(sizeof(COIString*) * context->numAllies);
  // First ally is always the player
  context->allyNames[0] = COIStringCreate(pInfo->name, 220, 40, context->textType);
  COIStringSetVisible(context->allyNames[0], false);
  allStrings[BATTLE_NUM_ACTIONS+context->numEnemies] = context->allyNames[0];
  for (int i = 1; i < context->numAllies; i++) {
    int allyType = context->allies[i]->actorType;
    char name[MAX_STRING_SIZE];
    sprintf(name, "%s %i", actorGetNameFromType(allyType), i + 1);
    context->allyNames[i] = COIStringCreate(name, 220, 40, context->textType);
    COIStringSetVisible(context->allyNames[i], false);
    allStrings[BATTLE_NUM_ACTIONS+context->numEnemies+i] = context->allyNames[i];
  }

  COIBoardSetStrings(board, allStrings, context->numStrings);
}

void _updateStatuses(BattleContext* context) {
  for (int i = 0; i < context->numAllies; i++) {
    AllyStatusUpdate(context->allyStatuses[i], context->allies[i]);
  }
}


COISprite** _getDynamicSprites(BattleContext* context) {
  COISprite** enemySprites = actorGetSpriteList(context->enemies, context->numEnemies);
  COISprite** allySprites = actorGetSpriteList(context->allies, context->numAllies);

  int numSprites = context->numEnemies + context->numAllies;
  COISprite** allSprites = malloc(sizeof(COISprite*) * numSprites);
  for (int i = 0; i < context->numEnemies; i++) {
    allSprites[i] = enemySprites[i];
  }
  for (int i = 0; i < context->numAllies; i++) {
    allSprites[context->numEnemies + i] = allySprites[i];
  }
  
  free(enemySprites);
  free(allySprites);

  return allSprites;
}

void _centerActorsInBox(Actor** allies, int numAllies, COISprite* box) {
  int boxCenterX = box->_x + box->_width / 2;
  for (int i = 0; i < numAllies; i++) {
    COISprite* actor = allies[i]->sprite;
    int actorOriginX = boxCenterX - actor->_width / 2;
    //COISpriteSetPos(allies[i]->sprite, BATTLE_A_OFFSET_X, BATTLE_OFFSET_Y + 80*i);
    COISpriteSetPos(actor, actorOriginX, box->_y + BATTLE_OFFSET_Y + (BATTLE_Y_STEP * i));
  }
}

COIBoard* battleCreateBoard(COIWindow* window, COIAssetLoader* loader,
			    COIBoard* outsideBoard, COILoop outsideLoop,
			    int enemyType, PlayerInfo* pInfo) {  
  COIBoard* board = COIBoardCreate(99, 91, 95, 225, 640, 480, loader);
  COIBoardLoadSpriteMap(board, COIWindowGetRenderer(window), "src/battle/spritemap.dat");

  // Context
  BattleContext* context = malloc(sizeof(BattleContext));
  context->board = board;
  context->pointingAtEnemies = false;
  context->targetedActorIndex = 0;
  context->turnIndex = 0;
  context->controlEnabled = true;
  context->currentActionIndex = 0;
  context->sceneStage = SS_MOVE_FORWARD;
  context->movementOffset = 0;
  context->pInfo = pInfo;
  context->summary = NULL;

  // Required for determining what to do after battle ends
  context->outside = outsideBoard;
  context->outsideLoop = outsideLoop;
  context->window = window;

  // Allies
  context->allies = pInfo->party;
  context->numAllies = pInfo->partySize;
  COISprite* aBox = COIBoardGetSprites(board)[BATTLE_SPRITEMAP_A_BOX];
  _centerActorsInBox(context->allies, context->numAllies, aBox);
  context->allyStatuses = malloc(sizeof(AllyStatus*) * context->numAllies);
  

  // Enemies, can later randomize number
  context->numEnemies = 3;

  // Actions
  context->actions = malloc(sizeof(BattleAction) *
			    (context->numAllies + context->numEnemies));

  // Keep a list of all strings we have that we can pass to the COIBoard
  context->numStrings = BATTLE_NUM_ACTIONS + context->numEnemies + context->numAllies;
  COIString** allStrings = malloc(sizeof(COIString*) * (BATTLE_NUM_ACTIONS + context->numEnemies + context->numAllies));

  context->textType = COITextTypeCreate(25, 255, 255, 255, COIWindowGetRenderer(window));
  
  COISprite** sprites = COIBoardGetSprites(board);

  // Pointer for enemies and allies
  context->pointer = sprites[5];
  context->pointer->_autoHandle = false;
  context->pointer->_visible = false;

  context->enemies = malloc(sizeof(Actor*) * context->numEnemies);
  for (int i = 0; i < context->numEnemies; i++) {
    context->enemies[i] = actorCreateOfType(enemyType, 0, 0, loader, window);
  }
  COISprite* eBox = COIBoardGetSprites(board)[BATTLE_SPRITEMAP_E_BOX];
  _centerActorsInBox(context->enemies, context->numEnemies, eBox);

  
  //COIBoardSetDynamicSprites(board, actorGetSpriteList(context->enemies, context->numEnemies), context->numEnemies);
  COIBoardSetDynamicSprites(board, _getDynamicSprites(context), context->numEnemies + context->numAllies);
  
  _makeStrings(context, pInfo, board);

  // Top-level menu
  context->actionMenu = COIMenuCreate(sprites[3], sprites[4]);
  COIMenuSetTexts(context->actionMenu, context->actionStrings, BATTLE_NUM_ACTIONS);

  // Submenu
  context->subMenu = COIMenuCreate(sprites[6], sprites[7]);
  COIMenuSetInvisible(context->subMenu);

  COIMenuSetVisible(context->actionMenu);
  context->menuFocus = ACTION_MENU;

  for (int i = 0; i < context->numAllies; i++) {
    context->allyStatuses[i] = AllyStatusCreate(context->board, window, 15);
    AllyStatusUpdate(context->allyStatuses[i], context->allies[i]);
  }

  COIBoardSetContext(board, (void*)context);


  return board;
}

// Adjust pointer sprite coords to be pointing at targeted actor
void _adjustPointer(BattleContext* context) {
  Actor** actors = context->pointingAtEnemies ? context->enemies : context->allies;
  
  COISprite* target =  actors[context->targetedActorIndex]->sprite;
  COISprite* pointer = context->pointer;
  
  int newY = target->_y + (target->_height / 2) - pointer->_height / 2;
  int newX = target->_x - BATTLE_POINTER_PADDING_X - pointer->_width;
  COISpriteSetPos(pointer, newX, newY);
}

COISprite* _toggleTargetNameVisibility(BattleContext* context, bool visible) {
  COIString** names = context->pointingAtEnemies ? context->enemyNames : context->allyNames;
  COIString* name = names[context->targetedActorIndex];
  COIStringSetVisible(name, visible);
}

int _getIndexAfterOffset(int index, int offset, int n) {
  // Loop around if we need to
  int newTargetIndex = index + offset;
  if (newTargetIndex < 0) {
    int offsetFromEnd = (-1 * newTargetIndex) % n;
    return (n - offsetFromEnd);
  } else {
    return (newTargetIndex % n);
  }
}

// Move pointer to actor 'offset' spaces away
void battleMovePointer(BattleContext* context, int offset) {
  int numActors = context->pointingAtEnemies ? context->numEnemies : context->numAllies;
  Actor** actors = context->pointingAtEnemies ? context->enemies : context->allies;
  if (numActors == 1) {
    return;
  }
  
  // Make name of the previous actor invisible
  _toggleTargetNameVisibility(context, false);
  
  int newTargetIndex = _getIndexAfterOffset(context->targetedActorIndex, offset, numActors);

  // Can't point over dead actor
  Actor* target = actors[newTargetIndex];
  while (actorIsDead(target)) {
    int direction = offset > 0 ? 1 : -1; // Move in direction of offset
    newTargetIndex = _getIndexAfterOffset(newTargetIndex, direction, numActors);
    target = actors[newTargetIndex];
  }

  context->targetedActorIndex = newTargetIndex;

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
  context->menuFocus = ACTION_MENU;
}

void _attack(BattleContext* context) {
  context->pointingAtEnemies = true;
  context->targetedActorIndex = 0;
  battleMovePointer(context, 0); // Move off of dead actor if we need to
  _adjustPointer(context);
  _toggleTargetNameVisibility(context, true);
  context->pointer->_visible = true;
}

void _tech(BattleContext* context) {
  // Display menu of TECH abilities in the future
  context->pointingAtEnemies = false;
  context->targetedActorIndex = 0;
  _adjustPointer(context);
  _toggleTargetNameVisibility(context, true);
  context->pointer->_visible = true;

  // Clean up previous COIStrings
  for (int i = 0; i < context->subMenu->_stringCount; i++) {
    COIBoardRemoveString(context->board, context->subMenu->_strings[i]);
    COIStringDestroy(context->subMenu->_strings[i]);
  }

  TechList* tList = context->allies[context->turnIndex]->techList;
  COIString** tNames = malloc(sizeof(COIString*) * tList->count);
  for (int i = 0; i <tList->count; i++) {
    tNames[i] = techNameAsCOIString(tList->techs[i], 0, 0, context->textType, tList->techs[i]->active);
    COIBoardAddString(context->board, tNames[i]);
  }
  COIMenuSetTexts(context->subMenu, tNames, tList->count);
  
  COIMenuSetVisible(context->subMenu);

  free(tNames);
}

void _pickNPCActions(BattleContext* context) {
  // Actions stored like this: |Ally|...|Ally|Enemy|...|Enemy|
  int offset = context->numAllies;
  for (int i = 0; i < context->numEnemies; i++) {
    context->actions[offset + i] = battleBehaviorGenerateAction(context->enemies[i], context->allies, context->numAllies, context->enemies, context->numEnemies);
  }
}

void _techSelection(BattleContext* context) {
  Actor* ally = context->allies[context->turnIndex];
  int selectedTech = context->subMenu->_current;
  TechList* tList = ally->techList;
  Tech* tech = tList->techs[selectedTech];
  COIString** tNames = context->subMenu->_strings;
  if (tech->active) {
    tech->active = false;
    COIBoardRemoveString(context->board, tNames[selectedTech]);
    COIStringDestroy(tNames[selectedTech]);
    tNames[selectedTech] = techNameAsCOIString(tech, 0, 0, context->textType, tech->active);
    COIBoardAddString(context->board, tNames[selectedTech]);
    COIMenuSetVisible(context->subMenu);
  } else if (ally->tp >= tech->cost) {
    tech->active = true;
    COIBoardRemoveString(context->board, tNames[selectedTech]);
    COIStringDestroy(tNames[selectedTech]);
    tNames[selectedTech] = techNameAsCOIString(tech, 0, 0, context->textType, tech->active);
    COIBoardAddString(context->board, tNames[selectedTech]);
    COIMenuSetVisible(context->subMenu);
  }
}

void _processActions(BattleContext* context) {
  int numActions = context->numAllies + context->numEnemies;

  // NPCs select actions
  _pickNPCActions(context);
  
  // Sort current BattleActions by actor AGI
  battleBehaviorSortActions(context->actions, numActions);
}

// Change the current ally index by 'step'.
// If we're moving past the last ally, process all actions
void _changeTurn(BattleContext* context, int step) {
  if (context->turnIndex + step >= context->numAllies) {
    context->turnIndex = 0;
    _processActions(context);
    // Ready to run the whole turn
    context->controlEnabled = false;
    context->pointer->_visible = false;
  } else if (context->turnIndex + step < 0) {
    // Can't go lower than ally at position 0
    context->turnIndex = 0;
  } else {
    context->turnIndex += step;
  }
}

void _selectAttackTarget(BattleContext* context) {
  BattleAction* action = &context->actions[context->turnIndex];
  Actor* actor = context->allies[context->turnIndex];
  action->actor = actor;
  action->target = context->enemies[context->targetedActorIndex];
  action->type = ATTACK;
  action->index = -1; // Unused
}

// Return true if we're done moving
bool _moveActorBackwards(BattleContext* context, Actor* actor) {
  // If actor is dead, don't do anything
  if (actorIsDead(actor)) {
    return true;
  }
  
  // If actor's an enemy, decrease x. If it's an ally, increase x.
  if (actor->sprite->_x + BATTLE_MAX_MOVEMENT < BATTLE_A_OFFSET_X) {
    COIBoardMoveSprite(context->board, actor->sprite,-1 * BATTLE_MOVEMENT_STEP, 0);
  } else {
    COIBoardMoveSprite(context->board, actor->sprite, BATTLE_MOVEMENT_STEP, 0);
  }
  context->movementOffset -= BATTLE_MOVEMENT_STEP;
  return context->movementOffset <= 0;
}

// Return true if we're done moving
bool _moveActorForward(BattleContext* context, Actor* actor) {
  // If actor is dead, don't do anything
  if (actorIsDead(actor)) {
    return true;
  }
  
  // If actor's an enemy, increase x. If it's an ally, decrease x.
  if (actor->sprite->_x + BATTLE_MAX_MOVEMENT < BATTLE_A_OFFSET_X) {
    COIBoardMoveSprite(context->board, actor->sprite, BATTLE_MOVEMENT_STEP, 0);
  } else {
    COIBoardMoveSprite(context->board, actor->sprite, -1 * BATTLE_MOVEMENT_STEP, 0);
  }
  context->movementOffset += BATTLE_MOVEMENT_STEP;
  return context->movementOffset >= BATTLE_MAX_MOVEMENT;
}

int _countAliveActors(Actor** actors, int numActors) {
  int aliveActors = 0;
  for (int i = 0; i < numActors; i++) {
    if (!actorIsDead(actors[i])) {
      aliveActors++;
    }
  }

  return aliveActors;
}

bool battleFinished(BattleContext* context) {
  if (_countAliveActors(context->allies, context->numAllies) == 0) {
    printf("Allies lose.\n");
    return true;
  }
  if(_countAliveActors(context->enemies, context->numEnemies) == 0) {
    printf("Enemies lose.\n");
    return true;
  }
  return false;
}


// When selecting what character should do, handle each option in menu.
bool battleHandleActionSelection(BattleContext* context) {
  switch (context->actionMenu->_current) {
  case BATTLE_ATTACK:
    _attack(context);
    context->menuFocus = ACTORS;
    break;
  case BATTLE_TECH:
    _tech(context);
    context->menuFocus = SUB_MENU;
    context->subMenuType = SM_TECH;
    break;
  case BATTLE_FLEE:
    // Replace this with probability check, flee may fail
    return true;
  default:
    printf("Invalid action in battle.\n");
    return false;
  }
  
  
  return false;
}

// User selects an actor when attacking, using item, etc.
void battleHandleActorSelect(BattleContext* context) {
  switch (context->actionMenu->_current) {
  case BATTLE_ATTACK:
    _selectAttackTarget(context);
    break;
  default:
    printf("Invalid actor selection in battle.\n");
    break;
  }
  _changeTurn(context, 1);
}


// User presses 'LEFT' key, cancels out of current operation
void battleHandleBack(BattleContext* context) {
  switch (context->actionMenu->_current) {
  case BATTLE_ATTACK:
    _focusActionMenu(context);
    break;
  case BATTLE_TECH:
    COIMenuSetInvisible(context->subMenu);
    _focusActionMenu(context);
    break;
  default:
    printf("Invalid action in battle.\n");
  }
}

void battleHandleSubMenuSelection(BattleContext* context) {
  switch (context->subMenuType) {
  case SM_TECH:
    _techSelection(context);
    break;
  default:
    printf("Error on battle submenu selection.\n");
  }
}


// Returns true if battle is finished
bool battleAdvanceScene(BattleContext* context) {
  int numActions = context->numAllies + context->numEnemies;
  if (context->currentActionIndex >= numActions) {
    // We're done processing actions, user can control again
    context->sceneStage = SS_MOVE_FORWARD;
    context->currentActionIndex = 0;
    context->controlEnabled = true;
    context->movementOffset = 0;
    context->pointer->_visible = true;
    _focusActionMenu(context);
  } else {
    BattleAction action = context->actions[context->currentActionIndex];
    // Actor may die before it's able to take its turn. Move to next action
    if (actorIsDead(action.actor)) {
      context->sceneStage = SS_MOVE_FORWARD;
      context->currentActionIndex++;
      return false;
    }
    switch (context->sceneStage) {
    case SS_MOVE_FORWARD:
      if (_moveActorForward(context, action.actor)) {
	context->sceneStage = SS_TEXT;
      }
      break;
    case SS_TEXT:
      if (!context->summary) {
	// Create ActionSummary. This holds the COIStrings
	// that describe the current action.
	COISprite* box = COIBoardGetSprites(context->board)[BATTLE_SPRITEMAP_DESC_BOX];
	context->summary = battleBehaviorDoAction(&action, context->pInfo->name, context->textType, context->board, box);
      } else if (context->summary->finished) {
	ActionSummaryDestroy(context->summary, context->board);
	context->summary = NULL;
	context->sceneStage = SS_MOVE_BACKWARDS;
      } else {
	ActionSummaryAdvance(context->summary);
      }
      
      break;
    case SS_MOVE_BACKWARDS:
      if (_moveActorBackwards(context, action.actor)) {
	context->sceneStage = SS_MOVE_FORWARD;
	// If we're done, move to next action
	context->currentActionIndex++;

	_updateStatuses(context);


	if (battleFinished(context)) {
	  return true;
	}
      }
      break;
    default:
      printf("Invalid scene stage.\n");
    }
  }

  return false;
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
  for (int i = 0; i < context->numAllies; i++) {
    AllyStatusDestroy(context->allyStatuses[i]);
  }
  free(context->allyStatuses);
  free(context->enemies);
  free(context->actions);
  free(context);
  COIBoardDestroy(board);
}
