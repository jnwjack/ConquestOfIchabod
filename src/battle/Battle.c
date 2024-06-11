#include "Battle.h"
#include "../actor.h"
#include "../special.h"

#define THICK_GRASS_ENEMY_COUNT 5
static int THICK_GRASS_ENEMY_TYPES[THICK_GRASS_ENEMY_COUNT] = { ACTOR_SKELETON, ACTOR_BOOWOW, ACTOR_TENTACLE, ACTOR_WIRE_MOTHER, ACTOR_VOLCANETTE };

static int _getNumStrings(BattleContext* context) {
  int count = BATTLE_NUM_ACTIONS + context->numEnemies + context->numAllies;

  return count;
}

static void _makeStrings(BattleContext* context, PlayerInfo* pInfo, COIBoard* board) {
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
    //char name[MAX_STRING_SIZE];
    //sprintf(name, "%s %i", actorGetNameFromType(enemy->actorType), i + 1);
    context->enemyNames[i] = COIStringCreate(actorGetNameFromType(enemy->actorType), 220, 40, context->textType);
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

static void _updateStatuses(BattleContext* context) {
  for (int i = 0; i < context->numAllies; i++) {
    AllyStatusUpdate(context->allyStatuses[i], context->allies[i]);
  }
}


static COISprite** _getPersistentSprites(BattleContext* context) {
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

static void _centerActorsInBox(Actor** allies, int numAllies, COISprite* box) {
  int boxCenterX = box->_x + box->_width / 2;
  for (int i = 0; i < numAllies; i++) {
    COISprite* actor = allies[i]->sprite;
    int actorOriginX = boxCenterX - actor->_width / 2;
    //COISpriteSetPos(allies[i]->sprite, BATTLE_A_OFFSET_X, BATTLE_OFFSET_Y + 80*i);
    COISpriteSetPos(actor, actorOriginX, box->_y + BATTLE_OFFSET_Y + (BATTLE_Y_STEP * i));
  }
}

static int _numEnemiesFromTerrain(Terrain terrain) {
  switch (terrain) {
  case TT_THICK_GRASS:
    return generateRandomCharInRange(1, 5);
    //return (generateRandomChar() % 5) + 1;
  case TT_TENTACLE:
    return 4;
  default:
    printf("Invalid terrain type for battle enemy count\n");
    return 0;
  }
  
}

static int _enemyTypeFromTerrain(Terrain terrain) {
  switch (terrain) {
  case TT_THICK_GRASS:
    {
      int randIndex = (generateRandomChar() % THICK_GRASS_ENEMY_COUNT);
      return THICK_GRASS_ENEMY_TYPES[randIndex];
    }
  case TT_TENTACLE:
    return ACTOR_TENTACLE;
  default:
    printf("Invalid terrain type for battle actor type\n");
    return ACTOR_SKELETON;
  }
}

unsigned long _xpYieldFromEnemyType(int enemyType) {
  switch (enemyType) {
  case ACTOR_SKELETON:
    return 15;
  case ACTOR_BOOWOW:
    return 25;
  case ACTOR_TENTACLE:
    return 50;
  case ACTOR_WIRE_MOTHER:
    return 35;
  case ACTOR_VOLCANETTE:
    return 30;
  default:
    return 0;
  }
}

unsigned int _goldFromEnemyType(int enemyType) {
  switch (enemyType) {
  case ACTOR_SKELETON:
    return (unsigned int)generateRandomCharInRange(3, 10);
  case ACTOR_BOOWOW:
    return (unsigned int)generateRandomCharInRange(0, 5);
  case ACTOR_TENTACLE:
    return (unsigned int)generateRandomCharInRange(10, 20);
  case ACTOR_WIRE_MOTHER:
    return (unsigned int)generateRandomCharInRange(8, 15);
  case ACTOR_VOLCANETTE:
    return (unsigned int)generateRandomCharInRange(15, 30);
  default:
    return 0;
  }
}

COIBoard* battleCreateBoard(COIWindow* window, COIAssetLoader* loader,
			    COIBoard* outsideBoard, COILoop outsideLoop,
			    Terrain terrain, PlayerInfo* pInfo) {  
  COIBoard* board = COIBoardCreate(2, 132, 28, 225, 640, 480, loader);
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
  context->splash = NULL;
  context->xpYield = 0;
  context->gold = 0;
  
  // Required for determining what to do after battle ends
  context->outside = outsideBoard;
  context->outsideLoop = outsideLoop;
  context->window = window;
  context->playerOutsideX = pInfo->party[0]->sprite->_x;
  context->playerOutsideY = pInfo->party[0]->sprite->_y;

  // Allies
  context->allies = pInfo->party;
  context->numAllies = pInfo->partySize;
  for (int i = 0; i < context->numAllies; i++) {
    actorFaceLeft(context->allies[i]);
    
  }
  COISprite* aBox = COIBoardGetSprites(board)[BATTLE_SPRITEMAP_A_BOX];
  _centerActorsInBox(context->allies, context->numAllies, aBox);
  aBox->_autoHandle = false;
  aBox->_visible = false;
  context->allyStatuses = malloc(sizeof(AllyStatus*) * context->numAllies);
  
  COISprite* splashBox = COIBoardGetSprites(context->board)[BATTLE_SPRITEMAP_SPLASH_BOX];
  splashBox->_autoHandle = false;
  splashBox->_visible = false;

  COISprite* descBox = COIBoardGetSprites(context->board)[BATTLE_SPRITEMAP_DESC_BOX];
  descBox->_autoHandle = false;
  descBox->_visible = false;


  // Enemies, can later randomize number
  context->numEnemies = _numEnemiesFromTerrain(terrain);

  // Actions
  context->actions = malloc(sizeof(BattleAction) *
			    (context->numAllies + context->numEnemies));

  // Keep a list of all strings we have that we can pass to the COIBoard
  context->numStrings = BATTLE_NUM_ACTIONS + context->numEnemies + context->numAllies;
  COIString** allStrings = malloc(sizeof(COIString*) * (BATTLE_NUM_ACTIONS + context->numEnemies + context->numAllies));

  context->textType = COITextTypeCreate(25, 255, 255, 255, COIWindowGetRenderer(window));
  
  COISprite** sprites = COIBoardGetSprites(board);

  // Pointer for enemies and allies
  context->pointer = sprites[BATTLE_SPRITEMAP_POINTER];
  context->pointer->_autoHandle = false;
  context->pointer->_visible = false;

  context->enemies = malloc(sizeof(Actor*) * context->numEnemies);
  for (int i = 0; i < context->numEnemies; i++) {
    int enemyType = _enemyTypeFromTerrain(terrain);
    context->enemies[i] = actorCreateOfType(enemyType, 0, 0, loader, window);
    context->xpYield += _xpYieldFromEnemyType(enemyType);
    context->gold += _goldFromEnemyType(enemyType);
    actorFaceRight(context->enemies[i]);
  }
  COISprite* eBox = COIBoardGetSprites(board)[BATTLE_SPRITEMAP_E_BOX];
  eBox->_autoHandle = false;
  eBox->_visible = false;
  _centerActorsInBox(context->enemies, context->numEnemies, eBox);
  
  COIBoardSetPersistentSprites(board, _getPersistentSprites(context), context->numEnemies + context->numAllies);

  _makeStrings(context, pInfo, board);

  // Name box
  sprites[BATTLE_SPRITEMAP_NAME_BOX]->_autoHandle = false;
  sprites[BATTLE_SPRITEMAP_NAME_BOX]->_visible = true;

  // Top-level menu
  sprites[BATTLE_SPRITEMAP_MENU_FRAME]->_autoHandle = false;
  sprites[BATTLE_SPRITEMAP_MENU_FRAME]->_visible = true;
  context->actionMenu = COIMenuCreate(sprites[BATTLE_SPRITEMAP_MENU_FRAME], sprites[BATTLE_SPRITEMAP_MENU_POINTER]);
  COIMenuSetTexts(context->actionMenu, context->actionStrings, BATTLE_NUM_ACTIONS);

  // Submenu
  //context->subMenu = COIMenuCreate(sprites[6], sprites[7]);
  context->subMenu = COIMenuCreateWithCapacity(sprites[BATTLE_SPRITEMAP_SUBMENU_FRAME], sprites[BATTLE_SPRITEMAP_SUBMENU_POINTER], MAX_TECH_COUNT_ALLY);
  COIMenuSetInvisible(context->subMenu);

  COIMenuSetVisible(context->actionMenu);
  context->menuFocus = ACTION_MENU;

  // Things next to each ally. Status strings and tech particles
  context->techParticles = malloc(sizeof(COISprite*) * context->numAllies);
  for (int i = 0; i < context->numAllies; i++) {
    context->allyStatuses[i] = AllyStatusCreate(context->board, window, 15);
    AllyStatusUpdate(context->allyStatuses[i], context->allies[i]);
    context->techParticles[i] = COISpriteCreateFromAssetID(context->allies[i]->sprite->_x - 9,
							   context->allies[i]->sprite->_y - 9,
							   50,
							   50,
							   COI_GLOBAL_LOADER,
							   34,
							   COIWindowGetRenderer(COI_GLOBAL_WINDOW));
    // JNW: could be cleaned up
    context->techParticles[i]->viewWindowWidth = 50;
    context->techParticles[i]->viewWindowHeight = 50;
    context->techParticles[i]->_autoHandle = false;
    context->techParticles[i]->_visible = false;
    COISpriteSetSheetIndex(context->techParticles[i], 0, 0);
    COIBoardAddDynamicSprite(board, context->techParticles[i]);
  }

  context->levelUpSplash = NULL;

  COIBoardSetContext(board, (void*)context);

  return board;
}

// Adjust pointer sprite coords to be pointing at targeted actor
static void _adjustPointer(BattleContext* context) {
  Actor** actors = context->pointingAtEnemies ? context->enemies : context->allies;
  
  COISprite* target =  actors[context->targetedActorIndex]->sprite;
  COISprite* pointer = context->pointer;
  
  int newY = target->_y + (target->_height / 2) - pointer->_height / 2;
  int newX = target->_x - BATTLE_POINTER_PADDING_X - pointer->_width;
  COISpriteSetPos(pointer, newX, newY);
}

static void _toggleTargetNameVisibility(BattleContext* context, bool visible) {
  COIString** names = context->pointingAtEnemies ? context->enemyNames : context->allyNames;
  COIString* name = names[context->targetedActorIndex];
  COIStringSetVisible(name, visible);
}

static int _getIndexAfterOffset(int index, int offset, int n) {
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

static void _disableTechParticlesIfNecessary(BattleContext* context, int index) {
  Actor* ally = context->allies[index];
  TechList* techList = ally->techList;
  for (int i = 0; i < techList->count; i++) {
    if (techList->techs[i]->active) {
      return;
    }
  }

  context->techParticles[index]->_visible = false;
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

void _item(BattleContext* context) {
  COIMenuFreeComponents(context->subMenu, context->board);
  //  ItemList* itemList = context->pInfo->inventory->items;
  Item** items = context->pInfo->inventory->backpack;
  for (int i = 0; i < context->pInfo->inventory->numBackpackItems; i++) {
    if (ItemCanUseInBattle(items[i])) {
      COIString* string = COIStringCreate(ItemListStringFromItemID(items[i]->id),
					  0, 0,
					  context->textType);
      COIStringSetVisible(string, true);
      COIBoardAddString(context->board, string);
      COIMenuAddString(context->subMenu, string, i);
    }
  }

  COIMenuSetVisible(context->subMenu);
}

void _special(BattleContext* context) {
  // Clean up previous COIStrings
  COIMenuFreeComponents(context->subMenu, context->board);

  // JNW: Using new menu functions. Should port the _tech function similarly
  IntList* specials = &context->allies[context->turnIndex]->specials;
  for (int i = 0; i < specials->length; i++) {
    COIString* string = COIStringCreate(specialName(specials->values[i]),
					0, 0,
					context->textType);
    COIStringSetVisible(string, true);
    COIBoardAddString(context->board, string);
    COIMenuAddString(context->subMenu, string, specials->values[i]);
  }
  
  COIMenuSetVisible(context->subMenu);
}

void _tech(BattleContext* context) {
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

void _itemSelection(BattleContext* context) {
  if (context->subMenu->_stringCount > 0) {
    context->pointingAtEnemies = false;
    context->targetedActorIndex = 0;
    _adjustPointer(context);
    _toggleTargetNameVisibility(context, true);
    context->pointer->_visible = true;
    COIMenuSetInvisible(context->subMenu);
    context->menuFocus = ACTORS;
  }
}

void _specialSelection(BattleContext* context) {
  Actor* ally = context->allies[context->turnIndex];
  int special = COIMenuGetCurrentValue(context->subMenu);
  
  context->pointingAtEnemies = specialTargetsEnemies(special);
  context->targetedActorIndex = 0;
  if (specialCost(special) <= ally->sp) {
    _adjustPointer(context);
    _toggleTargetNameVisibility(context, true);
    battleMovePointer(context, 0); // Move off dead actor if we need to
    context->pointer->_visible = true;
    COIMenuSetInvisible(context->subMenu);
    context->menuFocus = ACTORS;
  }
}


void _techSelection(BattleContext* context) {
  Actor* ally = context->allies[context->turnIndex];
  int selectedTech = context->subMenu->_current;
  TechList* tList = ally->techList;
  Tech* tech = tList->techs[selectedTech];
  COIString** tNames = context->subMenu->_strings;

  int activeTPCost = 0;
  for (int i = 0; i < tList->count; i++) {
    if (tList->techs[i]->active) {
      activeTPCost += tList->techs[i]->cost;
    }
  }
  
  if (tech->active) {
    tech->active = false;
    COIBoardRemoveString(context->board, tNames[selectedTech]);
    COIStringDestroy(tNames[selectedTech]);
    tNames[selectedTech] = techNameAsCOIString(tech, 0, 0, context->textType, tech->active);
    COIBoardAddString(context->board, tNames[selectedTech]);
    COIMenuSetVisible(context->subMenu);
    _disableTechParticlesIfNecessary(context, context->turnIndex);
  } else if (ally->tp >= tech->cost + activeTPCost) {
    tech->active = true;
    COIBoardRemoveString(context->board, tNames[selectedTech]);
    COIStringDestroy(tNames[selectedTech]);
    tNames[selectedTech] = techNameAsCOIString(tech, 0, 0, context->textType, tech->active);
    COIBoardAddString(context->board, tNames[selectedTech]);
    COIMenuSetVisible(context->subMenu);
    context->techParticles[context->turnIndex]->_visible = true;
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
    // Make action menu invisible
    COIMenuSetInvisible(context->actionMenu);
    COISprite** sprites = COIBoardGetSprites(context->board);
    sprites[BATTLE_SPRITEMAP_NAME_BOX]->_visible = false;
    _toggleTargetNameVisibility(context, false);
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

void _selectItemTarget(BattleContext* context) {
  BattleAction* action = &context->actions[context->turnIndex];
  Actor* actor = context->allies[context->turnIndex];
  action->actor = actor;
  action->target = context->allies[context->turnIndex];
  action->type = ITEM;
  Item** items = context->pInfo->inventory->backpack;
  action->index = items[COIMenuGetCurrentValue(context->subMenu)]->id;
}

void _selectSpecialTarget(BattleContext* context) {
  BattleAction* action = &context->actions[context->turnIndex];
  int numActors = context->pointingAtEnemies ? context->numEnemies : context->numAllies;
  Actor** actors = context->pointingAtEnemies ? context->enemies : context->allies;

  Actor* actor = context->allies[context->turnIndex];
  action->actor = actor;
  action->target = actors[context->targetedActorIndex];
  action->type = SPECIAL;
  action->index = COIMenuGetCurrentValue(context->subMenu);
}

// Return true if we're done moving
bool _moveActorBackwards(BattleContext* context, Actor* actor) {
  // If actor is dead, don't do anything
  if (actorIsDead(actor)) {
    return true;
  }
  
  // If actor's an enemy, decrease x. If it's an ally, increase x.
  if (actor->sprite->_x + BATTLE_MAX_MOVEMENT < BATTLE_A_OFFSET_X) {
    //COIBoardMoveSprite(context->board, actor->sprite,-1 * BATTLE_MOVEMENT_STEP, 0);
    actorMove(actor, -1 * BATTLE_MOVEMENT_STEP, 0, context->board);
    
  } else {
    //COIBoardMoveSprite(context->board, actor->sprite, BATTLE_MOVEMENT_STEP, 0);
    actorMove(actor, BATTLE_MOVEMENT_STEP, 0, context->board);
    // Move tech particles
    COIBoardMoveSprite(context->board, context->techParticles[0], BATTLE_MOVEMENT_STEP, 0);
  }
  context->movementOffset -= BATTLE_MOVEMENT_STEP;
  
  bool done =  context->movementOffset <= 0;
  if (done) {
    // Face the proper direction when movement's done
    actorTurnAround(actor);
  }
  return done;
}

// Return true if we're done moving
bool _moveActorForward(BattleContext* context, Actor* actor) {
  // If actor is dead, don't do anything
  if (actorIsDead(actor)) {
    return true;
  }
  
  // If actor's an enemy, increase x. If it's an ally, decrease x.
  if (actor->sprite->_x + BATTLE_MAX_MOVEMENT < BATTLE_A_OFFSET_X) {
    //COIBoardMoveSprite(context->board, actor->sprite, BATTLE_MOVEMENT_STEP, 0);
    actorMove(actor, BATTLE_MOVEMENT_STEP, 0, context->board);
  } else {
    //COIBoardMoveSprite(context->board, actor->sprite, -1 * BATTLE_MOVEMENT_STEP, 0);
    actorMove(actor, -1 * BATTLE_MOVEMENT_STEP, 0, context->board);
    // Move tech particles
    COIBoardMoveSprite(context->board, context->techParticles[0], -1 * BATTLE_MOVEMENT_STEP, 0);
    // Move tech particles
  }
  context->movementOffset += BATTLE_MOVEMENT_STEP;
  return context->movementOffset >= BATTLE_MAX_MOVEMENT;
}

static void _showSplash(BattleContext* context, BattleResult result, bool levelUp) {
  COISprite* box = COIBoardGetSprites(context->board)[BATTLE_SPRITEMAP_SPLASH_BOX];
  context->splash = BattleSplashCreate(context->board,
				       context->textType,
				       box,
				       result == BR_WIN,
				       context->xpYield,
               levelUp,
               context->gold);
				    
}

static int _countAliveActors(Actor** actors, int numActors) {
  int aliveActors = 0;
  for (int i = 0; i < numActors; i++) {
    if (!actorIsDead(actors[i])) {
      aliveActors++;
    }
  }

  return aliveActors;
}


// Run this when the battle's over.
static void _disableAllTechs(Actor* actor) {
  TechList* techList = actor->techList;
  for (int i = 0; i < techList->count; i++) {
    techList->techs[i]->active = false;
  }
}

static void _disableTechsIfTooExpensive(Actor* actor) {
  // Currently only does this for the player. May need to update this to handle enemies
  // or other allies in the future.
  if (actor->actorType != ACTOR_PLAYER) {
    return;
  }
  int activeTechIndices[MAX_TECH_COUNT_ALLY];
  int numActiveTechs = 0;
  int total = 0;
  //Actor* actor = context->allies[0];
  TechList* techList = actor->techList;
  for (int i = 0; i < techList->count; i++) {
    Tech* tech = techList->techs[i];
    if (tech->active) {
      activeTechIndices[numActiveTechs++] = i;
      total += tech->cost;
    }
  }

  while (total > actor->tp) {
    Tech* tech = techList->techs[activeTechIndices[--numActiveTechs]];
    tech->active = false;
    total -= tech->cost;
  }
}

void battleTick(BattleContext* context) {
  // Sparkle effect when techs are active
  for (int i = 0; i < context->numAllies; i++) {
    COISprite* sprite = context->techParticles[i];
    if (sprite->_visible) {
      sprite->_animationTicks++;
      if (sprite->_visible && sprite->_animationTicks > BATTLE_TECH_PARTICLE_TICKS) {
	int oldCol = sprite->_srcRect->x / sprite->_srcRect->w;
	COISpriteSetSheetIndex(sprite, 0, (oldCol + 1) % 5);
	sprite->_animationTicks = 0;
	COIBoardQueueDraw(context->board);
      }
    }
  }
}

BattleResult battleFinished(BattleContext* context) {
  if (_countAliveActors(context->allies, context->numAllies) == 0) {
    return BR_LOSS;
  }
  if(_countAliveActors(context->enemies, context->numEnemies) == 0) {
    return BR_WIN;
  }
  return BR_CONTINUE;
}


// When selecting what character should do, handle each option in menu.
BattleResult battleHandleActionSelection(BattleContext* context) {
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
  case BATTLE_SPECIAL:
    _special(context);
    context->menuFocus = SUB_MENU;
    context->subMenuType = SM_SPECIAL;
    break;
  case BATTLE_ITEM:
    _item(context);
    context->menuFocus = SUB_MENU;
    context->subMenuType = SM_ITEM;
    break;
  case BATTLE_FLEE:
    // Replace this with probability check, flee may fail
    _disableAllTechs(context->allies[0]);
    return BR_FLEE;
  default:
    printf("Invalid action in battle.\n");
    return BR_CONTINUE;
  }
  
  
  return BR_CONTINUE;
}

// User selects an actor when attacking, using item, etc.
void battleHandleActorSelect(BattleContext* context) {
  switch (context->actionMenu->_current) {
  case BATTLE_ATTACK:
    _selectAttackTarget(context);
    break;
  case BATTLE_SPECIAL:
    _selectSpecialTarget(context);
    break;
  case BATTLE_ITEM:
    _selectItemTarget(context);
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
  case BATTLE_ITEM:
  case BATTLE_TECH:
  case BATTLE_SPECIAL:
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
  case SM_SPECIAL:
    _specialSelection(context);
    break;
  case SM_ITEM:
    _itemSelection(context);
    break;
  default:
    printf("Error on battle submenu selection.\n");
  }
}


// Returns true if battle is finished
BattleResult battleAdvanceScene(BattleContext* context, bool selection) {
  int numActions = context->numAllies + context->numEnemies;
  
  if(context->sceneStage == SS_SPLASH) {
    // If the splash screen has finished animating
    if (BattleSplashFinished(context->splash)) {
      BattleResult battleResult = battleFinished(context);
      if (battleResult == BR_WIN) {
        unsigned int oldLevel = context->pInfo->level;
        playerAddXP(context->pInfo, context->xpYield);
        context->pInfo->inventory->money = MIN(MAX_MONEY, context->pInfo->inventory->money + (int)context->gold);
        if (context->pInfo->level > oldLevel) {
          context->levelUpSplash = LevelUpSplashCreate(context->board, context->pInfo);
          context->menuFocus = LEVEL_UP;
          context->controlEnabled = true;
          for (int i = 0; i < context->numAllies; i++) {
            AllyStatusSetVisible(context->allyStatuses[i], false);
          }
          context->board->_sprites[BATTLE_SPRITEMAP_NAME_BOX]->_visible = false;
          BattleSplashDestroy(context->splash, context->board);
          return BR_CONTINUE;
        }
      }
      BattleSplashDestroy(context->splash, context->board);
      context->splash = NULL;
      return battleFinished(context);
    } else {
      BattleSplashAnimate(context->splash, selection);
      return BR_CONTINUE;
    }
  } else if (context->currentActionIndex >= numActions) {
    // We're done processing actions, user can control again

    // Disable TECHs that we will not be able to afford next turn
    // Only doing this for the player right now. Not handling other allies or enemies
    _disableTechsIfTooExpensive(context->allies[0]);
    _disableTechParticlesIfNecessary(context, 0);
    
    context->sceneStage = SS_MOVE_FORWARD;
    context->currentActionIndex = 0;
    context->controlEnabled = true;
    COIMenuSetVisible(context->actionMenu);
    COISprite** sprites = COIBoardGetSprites(context->board);
    sprites[BATTLE_SPRITEMAP_NAME_BOX]->_visible = true;
    context->movementOffset = 0;
    context->pointer->_visible = true;
    _focusActionMenu(context);
  } else {
    BattleAction action = context->actions[context->currentActionIndex];
    // Actor may die before it's able to take its turn. Move to next action
    if (actorIsDead(action.actor)) {
      context->sceneStage = SS_MOVE_FORWARD;
      context->currentActionIndex++;
      return BR_CONTINUE;
    }
    switch (context->sceneStage) {
    case SS_MOVE_FORWARD:
      if (_moveActorForward(context, action.actor)) {
	context->sceneStage = SS_TEXT;
      }
      break;
    case SS_TEXT:
      if (!context->summary) {
	// We're working with the target's sprite in this section
	action.target->sprite->_autoHandle = false;
	// Create ActionSummary. This holds the COIStrings
	// that describe the current action.
	COISprite* box = COIBoardGetSprites(context->board)[BATTLE_SPRITEMAP_DESC_BOX];
	box->_visible = true;
	context->summary = battleBehaviorDoAction(&action, context->textType, context->board, box, context->pInfo);
	// If it's a item, remove it from backpack on use
	if (action.type == ITEM) {
	  inventoryRemoveBackpackItemFirstInstance(context->pInfo->inventory,
						   ItemListGetItem(context->pInfo->inventory->items,
								   action.index));
	}
      } else if (context->summary->finished) {
	ActionSummaryDestroy(context->summary, context->board);
	context->summary = NULL;
	context->sceneStage = SS_MOVE_BACKWARDS;
	COISprite* box = COIBoardGetSprites(context->board)[BATTLE_SPRITEMAP_DESC_BOX];
	box->_visible = false;
	// We're done with the target's sprite
	if (actorIsDead(action.target)) {
	  action.target->sprite->_visible = false;
	} else {
	  action.target->sprite->_autoHandle = true;
	}
      } else {
	ActionSummaryAdvance(context->summary, selection);
	// Flicker effect on target actor
	if (context->summary->currentString > 0) {
	  action.target->sprite->_visible = true;
	}
	else if (context->summary->ticks % 10 == 0) {
	  action.target->sprite->_visible = !action.target->sprite->_visible;
	}
      }
      
      break;
    case SS_MOVE_BACKWARDS:
      if (_moveActorBackwards(context, action.actor)) {
	actorStandStill(action.actor);
	context->sceneStage = SS_MOVE_FORWARD;
	// If we're done, move to next action
	context->currentActionIndex++;

	_updateStatuses(context);

	BattleResult res = battleFinished(context);
	
	// We're done with the battle, display the splash screen
	if (res != BR_CONTINUE) {
	  _disableAllTechs(context->allies[0]);
	  _showSplash(context, res, context->pInfo->xp + context->xpYield >= context->pInfo->xpForLevelUp);
	  context->sceneStage = SS_SPLASH;
	}
      }
      break;

    default:
      printf("Invalid scene stage.\n");
    }
  }

  return BR_CONTINUE;
}


void battleDestroyBoard(COIBoard* board) {
  BattleContext* context = (BattleContext*)board->context;
  COIMenuDestroy(context->actionMenu);
  COITextTypeDestroy(context->textType);
  for (int i = 0; i < BATTLE_NUM_ACTIONS; i++) {
    COIStringDestroy(context->actionStrings[i]);
  }
  context->actionMenu->_strings = NULL;
  for (int i = 0; i < context->numEnemies; i++) {
    actorDestroy(context->enemies[i]);
    COIStringDestroy(context->enemyNames[i]);
  }
  for (int i = 0; i < context->numAllies; i++) {
    COIBoardRemoveDynamicSprite(board, context->techParticles[i]);
    COISpriteDestroy(context->techParticles[i]);
    AllyStatusDestroy(context->allyStatuses[i]);
  }
  if (context->levelUpSplash) {
    LevelUpSplashDestroy(context->levelUpSplash, board);
  }
  free(context->techParticles);
  free(context->allyStatuses);
  free(context->enemies);
  free(context->actions);
  free(context);
  COIBoardDestroy(board);
}
