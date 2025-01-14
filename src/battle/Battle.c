#include "Battle.h"
#include "../actor.h"
#include "../special.h"

#define BROWN_GRASS_ENEMY_COUNT 4
static int BROWN_GRASS_ENEMY_TYPES[BROWN_GRASS_ENEMY_COUNT] = { ACTOR_SKELETON, ACTOR_BOOWOW, ACTOR_WIRE_MOTHER, ACTOR_VOLCANETTE };

#define BROWN_GRASS_LATER_ENEMY_COUNT 9
static int BROWN_GRASS_LATER_ENEMY_TYPES[BROWN_GRASS_LATER_ENEMY_COUNT] = { ACTOR_SKELETON, ACTOR_BOOWOW, ACTOR_WIRE_MOTHER, ACTOR_VOLCANETTE, 
                                                                      ACTOR_MEAT_FLAYER, ACTOR_FEARWOLF, ACTOR_WIRE_MAIDEN, ACTOR_PYROID, ACTOR_TENTACLE };

#define BROWN_GRASS_LATEST_ENEMY_COUNT 5
static int BROWN_GRASS_LATEST_ENEMY_TYPES[BROWN_GRASS_LATEST_ENEMY_COUNT] = { ACTOR_MEAT_FLAYER, ACTOR_FEARWOLF, ACTOR_WIRE_MAIDEN, ACTOR_PYROID, ACTOR_TENTACLE };


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

  for (int i = 0; i < context->numStrings; i++) {
    COIBoardAddString(board, allStrings[i], 0);
  }

  // COIBoardSetStrings(board, allStrings, context->numStrings);
}

static void _updateStatuses(BattleContext* context) {
  for (int i = 0; i < context->numAllies; i++) {
    AllyStatusUpdate(context->allyStatuses[i], context->allies[i], context->modifiers);
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

static void _positionEnemies(Actor** enemies, int numEnemies) {
  for (int i = 0; i < numEnemies; i++) {
    int xOffset;
    int yOffset;
    int columnPosition;
    if (i < 2) {
      xOffset = 170;
      yOffset = 170;
      columnPosition = i;
    } else {
      xOffset = 70;
      yOffset = 138;
      if (i == 2) {
        columnPosition = 1;
      } else if (i == 3) {
        columnPosition = 0;
      } else {
        columnPosition = 2;
      }
    }

    COISpriteSetPos(enemies[i]->sprite, xOffset, yOffset + (BATTLE_Y_STEP * columnPosition));
  }
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
  case TT_BROWN_GRASS:
  case TT_THICK_GRASS_CORRUPT:
  case TT_BROWN_GRASS_CORRUPT:
  case TT_THICK_GRASS:
    if (GLOBAL_TIME.phase >= TS_EVENING) {
      return generateRandomCharInRange(1, 5);
    }
    return generateRandomCharInRange(1, 2);
  case TT_TENTACLE:
    return 5;
  default:
    printf("Invalid terrain type for battle enemy count\n");
    return 0;
  }
  
}

static int _enemyTypeFromTerrain(Terrain terrain) {
  switch (terrain) {
  case TT_THICK_GRASS_CORRUPT:
  case TT_THICK_GRASS:
    {
      if (GLOBAL_TIME.day > BATTLE_SET_2_DAYS) {
        int randIndex = (generateRandomChar() % BROWN_GRASS_LATEST_ENEMY_COUNT);
        return BROWN_GRASS_LATEST_ENEMY_TYPES[randIndex];
      } else if (GLOBAL_TIME.day > BATTLE_SET_1_DAYS) {
        int randIndex = (generateRandomChar() % BROWN_GRASS_LATER_ENEMY_COUNT);
        return BROWN_GRASS_LATER_ENEMY_TYPES[randIndex];
        // int randVal = generateRandomChar() % 2;
        // if (randVal == 1) {
        //   return ACTOR_FEARWOLF;
        // }
        // return ACTOR_BOOWOW;
      } else if (GLOBAL_TIME.day > BATTLE_GREEN_GRASS_EXPANDED_ENEMIES_DAYS) {
        int randIndex = (generateRandomChar() % BROWN_GRASS_ENEMY_COUNT);
        return BROWN_GRASS_ENEMY_TYPES[randIndex];
      }
      return ACTOR_BOOWOW;
    }
  case TT_BROWN_GRASS_CORRUPT:
  case TT_BROWN_GRASS:
    {
      if (GLOBAL_TIME.day > BATTLE_SET_2_DAYS) {
        int randIndex = (generateRandomChar() % BROWN_GRASS_LATEST_ENEMY_COUNT);
        return BROWN_GRASS_LATEST_ENEMY_TYPES[randIndex];
      } else if (GLOBAL_TIME.day > BATTLE_SET_1_DAYS) {
        int randIndex = (generateRandomChar() % BROWN_GRASS_LATER_ENEMY_COUNT);
        return BROWN_GRASS_LATER_ENEMY_TYPES[randIndex];
      }
      int randIndex = (generateRandomChar() % BROWN_GRASS_ENEMY_COUNT);
      return BROWN_GRASS_ENEMY_TYPES[randIndex];
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
    return 45;
  case ACTOR_BOOWOW:
    return 30;
  case ACTOR_TENTACLE:
    return 75;
  case ACTOR_WIRE_MOTHER:
    return 35;
  case ACTOR_VOLCANETTE:
    return 40;
  default:
    return 0;
  }
}

unsigned int _goldFromEnemyType(int enemyType) {
  switch (enemyType) {
  case ACTOR_SKELETON:
    return (unsigned int)generateRandomCharInRange(3, 7);
  case ACTOR_BOOWOW:
    return (unsigned int)generateRandomCharInRange(0, 5);
  case ACTOR_TENTACLE:
    return (unsigned int)generateRandomCharInRange(10, 20);
  case ACTOR_WIRE_MOTHER:
    return (unsigned int)generateRandomCharInRange(2, 6);
  case ACTOR_VOLCANETTE:
    return (unsigned int)generateRandomCharInRange(4, 8);
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

  if (terrain == TT_TENTACLE) {
    context->playerOutsideX = pInfo->party[0]->sprite->_x - 32;
  } else {
    context->playerOutsideX = pInfo->party[0]->sprite->_x;
  }
  context->playerOutsideY = pInfo->party[0]->sprite->_y;

  // Allies
  context->allies = pInfo->party;
  context->numAllies = pInfo->partySize;
  for (int i = 0; i < context->numAllies; i++) {
    actorFaceLeft(context->allies[i]);
  }
  COISprite* aBox = COISpriteCreateFromAssetID(450, 80, 100, 300, COI_GLOBAL_LOADER, 5, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COIBoardAddDynamicSprite(board, aBox, 0);
  _centerActorsInBox(context->allies, context->numAllies, aBox);
  aBox->_autoHandle = false;
  aBox->_visible = false;
  context->allyStatuses = malloc(sizeof(AllyStatus*) * context->numAllies);

  context->descBox = COISpriteCreateFromAssetID(320, 350, 300, 100, COI_GLOBAL_LOADER, 5, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COIBoardAddDynamicSprite(board, context->descBox, 0);
  context->descBox->_autoHandle = false;
  context->descBox->_visible = false;


  // Enemies, can later randomize number
  context->numEnemies = _numEnemiesFromTerrain(terrain);

  // Actions
  context->actions = malloc(sizeof(BattleAction) *
			    (context->numAllies + context->numEnemies));

  // Keep a list of all strings we have that we can pass to the COIBoard
  context->numStrings = BATTLE_NUM_ACTIONS + context->numEnemies + context->numAllies;
  COIString** allStrings = malloc(sizeof(COIString*) * (BATTLE_NUM_ACTIONS + context->numEnemies + context->numAllies));

  context->textType = COITextTypeCreate(16, 255, 255, 255, COIWindowGetRenderer(window));

  // Pointer for enemies and allies
  context->pointer = COISpriteCreateFromAssetID(320, 350, 32, 32, COI_GLOBAL_LOADER, 6, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COIBoardAddDynamicSprite(board, context->pointer, 0);
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
  // XP Bonus for fighting multiple enemies
  context->xpYield += (MAX(0, context->numEnemies - 1) * 10);
  _positionEnemies(context->enemies, context->numEnemies);
  
  COIBoardSetPersistentSprites(board, _getPersistentSprites(context), context->numEnemies + context->numAllies);

  _makeStrings(context, pInfo, board);

  // Name box
  context->nameBox = COISpriteCreateFromAssetID(210, 20, 200, 75, COI_GLOBAL_LOADER, 5, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COIBoardAddDynamicSprite(board, context->nameBox, 0);
  context->nameBox->_autoHandle = false;
  context->nameBox->_visible = true;

  // Top-level menu
  COISprite* topLevelMenuFrame = COISpriteCreateFromAssetID(400, 320, 200, 150, COI_GLOBAL_LOADER, 5, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COIBoardAddDynamicSprite(board, topLevelMenuFrame, 0);
  COISprite* topLevelPointer = COISpriteCreateFromAssetID(100, 200, 32, 32, COI_GLOBAL_LOADER, 6, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COIBoardAddDynamicSprite(board, topLevelPointer, 0);
  topLevelMenuFrame->_autoHandle = false;
  topLevelMenuFrame->_visible = true;
  topLevelPointer->_autoHandle = false;
  topLevelPointer->_visible = true;
  context->actionMenu = COIMenuCreate(topLevelMenuFrame, topLevelPointer);
  COIMenuSetTexts(context->actionMenu, context->actionStrings, BATTLE_NUM_ACTIONS);

  // Submenu
  //context->subMenu = COIMenuCreate(sprites[6], sprites[7]);
  COISprite* subMenuFrame = COISpriteCreateFromAssetID(115, 320, 275, 150, COI_GLOBAL_LOADER, 5, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COIBoardAddDynamicSprite(board, subMenuFrame, 0);
  COISprite* subMenuPointer = COISpriteCreateFromAssetID(100, 200, 32, 32, COI_GLOBAL_LOADER, 6, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COIBoardAddDynamicSprite(board, subMenuPointer, 0);
  context->subMenu = COIMenuCreateWithCapacity(subMenuFrame, subMenuPointer, MAX_TECH_COUNT_ALLY);
  COIMenuSetInvisible(context->subMenu);

  COIMenuSetVisible(context->actionMenu);
  context->menuFocus = ACTION_MENU;

  context->modifiers = LinkedListCreate();

  // Things next to each ally. Status strings and tech particles
  context->techParticles = malloc(sizeof(COISprite*) * context->numAllies);
  for (int i = 0; i < context->numAllies; i++) {
    context->allyStatuses[i] = AllyStatusCreate(context->board, window, 10);
    AllyStatusUpdate(context->allyStatuses[i], context->allies[i], context->modifiers);
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
    COIBoardAddDynamicSprite(board, context->techParticles[i], 0);
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

static int _getPointerIndexFromMove(int index, int move, int n) {
  // Special cases for left and right
  if (move == MOVING_LEFT) {
    if (n > 2 && index < 2) {
      return MAX(n - 1, (index + 3) % n);
    }
    return index;
  }
  if (move == MOVING_RIGHT) {
    if (index >= 2) {
      return MAX(0, index - 4);
    }
    return index;
  }

  // Now handle moving up and down
  if (index < 2) {
    int step = move == MOVING_UP ? -1 : 1;
    int next = index + step;
    if (next < 0) {
      return 1;
    }
    return next % 2;
  } else if (index == 2) { // Can move to 3 (up) or 4 (down).
    if (n == 3) {
      return index;
    } else if (n == 4) {
      return index + 1;
    } else {
      return move == MOVING_UP ? index + 1 : index + 2;
    }
  } else if (index == 3) { // Can move to 4 (looping around going up) or 2 (down).
    if (move == MOVING_DOWN || n < 5) {
      return 2;
    }    
    return 4;
  } else { // Can move to 3 (looping around going down) or 2 (up).
    return move == MOVING_UP ? index - 2 : index - 1;
  }
}

static bool _aliveActorsInLeftColumn(Actor** actors, int n) {
  for (int i = 2; i < n; i++) {
    if (!actorIsDead(actors[i])) {
      return true;
    }
  }
  return false;
}

static bool _aliveActorsInRightColumn(Actor** actors, int n) {
  for (int i = 0; i < MIN(2, n); i++) {
    if (!actorIsDead(actors[i])) {
      return true;
    }
  }
  return false;
}

static void _battleInitPointer(BattleContext* context) {
  int numActors = context->pointingAtEnemies ? context->numEnemies : context->numAllies;
  Actor** actors = context->pointingAtEnemies ? context->enemies : context->allies;
  if (numActors == 1) {
    return;
  }

  // Make name of the previous actor invisible
  _toggleTargetNameVisibility(context, false);

  int currentTargetIndex = 0;
  // Can't point over dead actor
  Actor* target = actors[currentTargetIndex];
  while (actorIsDead(target)) {
    currentTargetIndex = _getIndexAfterOffset(currentTargetIndex, 1, numActors);
    target = actors[currentTargetIndex];
  }
  context->targetedActorIndex = currentTargetIndex;
}

// Move pointer to actor 'offset' spaces away
void battleMovePointer(BattleContext* context, int direction) {
  int numActors = context->pointingAtEnemies ? context->numEnemies : context->numAllies;
  Actor** actors = context->pointingAtEnemies ? context->enemies : context->allies;
  if (numActors == 1) {
    return;
  }
  
  // Make name of the previous actor invisible
  _toggleTargetNameVisibility(context, false);
  
  int newTargetIndex = _getPointerIndexFromMove(context->targetedActorIndex, direction, numActors);

  // Can't point over dead actor
  Actor* target = actors[newTargetIndex];
  while (actorIsDead(target)) {
    // int direction = offset > 0 ? 1 : -1; // Move in direction of offset
    if (direction == MOVING_LEFT && _aliveActorsInLeftColumn(actors, numActors)) {
      direction = MOVING_DOWN;
      newTargetIndex = _getPointerIndexFromMove(newTargetIndex, direction, numActors);
    } else if (direction == MOVING_RIGHT &&  _aliveActorsInRightColumn(actors, numActors)) {
      direction = MOVING_DOWN;
      newTargetIndex = _getPointerIndexFromMove(newTargetIndex, direction, numActors);
    } else if (direction == MOVING_DOWN || direction == MOVING_UP) {
      newTargetIndex = _getPointerIndexFromMove(newTargetIndex, direction, numActors);
    } else {
      newTargetIndex = context->targetedActorIndex;
    }

    // newTargetIndex = _getIndexAfterOffset(newTargetIndex, direction, numActors);
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
  _battleInitPointer(context);
  _adjustPointer(context);
  _toggleTargetNameVisibility(context, true);
  context->pointer->_visible = true;
}

bool _item(BattleContext* context) {
  bool hasValidItems = false;
  Item** items = context->pInfo->inventory->backpack;
  for (int i = 0; i < context->pInfo->inventory->numBackpackItems; i++) {
    if (ItemCanUseInBattle(items[i])) {
      hasValidItems = true;
    }
  }

  if (!hasValidItems) {
    return false;
  }

  COIMenuFreeComponents(context->subMenu, context->board);
  //  ItemList* itemList = context->pInfo->inventory->items;
  for (int i = 0; i < context->pInfo->inventory->numBackpackItems; i++) {
    if (ItemCanUseInBattle(items[i])) {
      COIString* string = COIStringCreate(ItemListStringFromItemID(items[i]->id),
					  0, 0,
					  context->textType);
      COIStringSetVisible(string, true);
      COIBoardAddString(context->board, string, 0);
      COIMenuAddString(context->subMenu, string, i);
    }
  }

  COIMenuReset(context->subMenu);
  COIMenuSetVisible(context->subMenu);

  context->menuFocus = SUB_MENU;
  context->subMenuType = SM_ITEM;

  return true;
}

bool _special(BattleContext* context) {
  if (context->allies[context->turnIndex]->specials.length < 1) {
    return false;
  }

  // Clean up previous COIStrings
  COIMenuFreeComponents(context->subMenu, context->board);

  // JNW: Using new menu functions. Should port the _tech function similarly
  IntList* specials = &context->allies[context->turnIndex]->specials;
  char buf[MAX_STRING_SIZE];
  for (int i = 0; i < specials->length; i++) {
    snprintf(buf, MAX_STRING_SIZE, "%s-%i", specialName(specials->values[i]), specialCost(specials->values[i]));
    COIString* string = COIStringCreate(buf,
					0, 0,
					context->textType);
    COIStringSetVisible(string, true);
    COIBoardAddString(context->board, string, 0);
    COIMenuAddString(context->subMenu, string, specials->values[i]);
  }
  
  COIMenuReset(context->subMenu);
  COIMenuSetVisible(context->subMenu);

  context->menuFocus = SUB_MENU;
  context->subMenuType = SM_SPECIAL;
  return true;
}

bool _tech(BattleContext* context) {
  if (context->allies[context->turnIndex]->techList->count < 1) {
    return false;
  }

  COIMenuFreeComponents(context->subMenu, context->board);

  TechList* tList = context->allies[context->turnIndex]->techList;
  for (int i = 0; i < tList->count; i++) {
    COIString* string = techNameAsCOIStringWithCost(tList->techs[i], 0, 0, context->textType, tList->techs[i]->active);
    COIStringSetVisible(string, true);
    COIBoardAddString(context->board, string, 0);
    COIMenuAddString(context->subMenu, string, tList->techs[i]->id);
  }

  COIMenuReset(context->subMenu);
  COIMenuSetVisible(context->subMenu);

  context->menuFocus = SUB_MENU;
  context->subMenuType = SM_TECH;
  return true;
}

void _pickNPCActions(BattleContext* context) {
  // Actions stored like this: |Ally|...|Ally|Enemy|...|Enemy|
  int offset = context->numAllies;
  for (int i = 0; i < context->numEnemies; i++) {
    battleBehaviorGenerateAction(&context->actions[offset + i], context->enemies[i], context->allies, context->numAllies, context->enemies, context->numEnemies);
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
  
  int cost = specialCost(special);
  TechList* techList = context->allies[0]->techList;
  for (int i = 0; i < techList->count; i++) {
    Tech* tech = techList->techs[i];
    if (tech->id == TECH_ID_FOCUS && tech->active) {
      cost /= 2;
    }
  }
  if (cost <= ally->sp) {
    _battleInitPointer(context);
    _adjustPointer(context);
    _toggleTargetNameVisibility(context, true);
    context->pointer->_visible = true;
    COIMenuSetInvisible(context->subMenu);
    context->menuFocus = ACTORS;
    COISoundPlay(COI_SOUND_SELECT);
  } else {
    COISoundPlay(COI_SOUND_INVALID);
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
    COIBoardRemoveString(context->board, tNames[selectedTech], 0);
    COIStringDestroy(tNames[selectedTech]);
    tNames[selectedTech] = techNameAsCOIStringWithCost(tech, 0, 0, context->textType, tech->active);
    COIBoardAddString(context->board, tNames[selectedTech], 0);
    COIMenuSetVisible(context->subMenu);
    _disableTechParticlesIfNecessary(context, context->turnIndex);
    COISoundPlay(COI_SOUND_SELECT);
  } else if (ally->tp >= tech->cost + activeTPCost) {
    tech->active = true;
    COIBoardRemoveString(context->board, tNames[selectedTech], 0);
    COIStringDestroy(tNames[selectedTech]);
    tNames[selectedTech] = techNameAsCOIStringWithCost(tech, 0, 0, context->textType, tech->active);
    COIBoardAddString(context->board, tNames[selectedTech], 0);
    COIMenuSetVisible(context->subMenu);
    context->techParticles[context->turnIndex]->_visible = true;
    COISoundPlay(COI_SOUND_SELECT);
  } else {
    COISoundPlay(COI_SOUND_INVALID);
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
    context->nameBox->_visible = false;
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
  action->successfulFlee = false;
  action->numOtherTargets = 0;
  action->numOtherTargetsKilled = 0;
}

void _selectFlee(BattleContext* context) {
  BattleAction* action = &context->actions[context->turnIndex];
  Actor* actor = context->allies[context->turnIndex];
  action->actor = actor;
  // Randomly pick target to flee "against"
  int targetIndex = generateRandomCharInRange(0, context->numEnemies - 1);
  action->target = context->enemies[targetIndex];
  while (actorIsDead(action->target)) {
    targetIndex = (targetIndex + 1) % context->numEnemies;
    action->target = context->enemies[targetIndex];
  }
  action->type = FLEE;
  action->index = -1; // Unused
  action->successfulFlee = false;
  action->numOtherTargets = 0;
  action->numOtherTargetsKilled = 0;
}

void _selectItemTarget(BattleContext* context) {
  BattleAction* action = &context->actions[context->turnIndex];
  Actor* actor = context->allies[context->turnIndex];
  action->actor = actor;
  action->target = context->allies[context->turnIndex];
  action->type = ITEM;
  Item** items = context->pInfo->inventory->backpack;
  action->index = items[COIMenuGetCurrentValue(context->subMenu)]->id;
  action->successfulFlee = false;
  action->numOtherTargets = 0;
  action->numOtherTargetsKilled = 0;
}

void _selectSpecialTarget(BattleContext* context) {
  BattleAction* action = &context->actions[context->turnIndex];
  int numActors = context->pointingAtEnemies ? context->numEnemies : context->numAllies;
  Actor** actors = context->pointingAtEnemies ? context->enemies : context->allies;

  Actor* actor = context->allies[context->turnIndex];
  battleBehaviorMakeSpecial(action, COIMenuGetCurrentValue(context->subMenu), context->targetedActorIndex, actors, numActors, actor);
}

// Return true if we're done moving
bool _moveActorBackwards(BattleContext* context, Actor* actor) {
  // If actor is dead, don't do anything
  // if (actorIsDead(actor)) {
  //   return true;
  // }
  
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
  // if (actorIsDead(actor)) {
  //   return true;
  // }
  
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
  COISprite* splashBox = COISpriteCreateFromAssetID(185, 115, 260, 240, COI_GLOBAL_LOADER, 5, COIWindowGetRenderer(COI_GLOBAL_WINDOW));

  if (result == BR_WIN) { 
    COISoundPlay(COI_SOUND_CELEBRATION);
  } else if (result == BR_LOSS) {
    COISoundPlay(COI_SOUND_DEFEAT);
  }
  context->splash = BattleSplashCreate(context->board,
				       context->textType,
				       splashBox,
				       result == BR_WIN,
				       context->xpYield,
               levelUp,
               context->gold,
               context->enemies,
               context->numEnemies,
               context->pInfo->inventory);
				    
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
  bool successful = true;
  switch (context->actionMenu->_current) {
  case BATTLE_ATTACK:
    _attack(context);
    context->menuFocus = ACTORS;
    break;
  case BATTLE_TECH:
    successful = _tech(context);
    break;
  case BATTLE_SPECIAL:
    successful = _special(context);
    break;
  case BATTLE_ITEM:
    successful = _item(context);
    break;
  case BATTLE_FLEE:
    // Replace this with probability check, flee may fail
    _selectFlee(context);
    _changeTurn(context, 1);
    break;
  default:
    printf("Invalid action in battle.\n");
  }

  if (successful) {
    COISoundPlay(COI_SOUND_SELECT);
  } else {
    COISoundPlay(COI_SOUND_INVALID);
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


// User presses 'back' key, cancels out of current operation
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
          playerUpdateClassProgressionFromTime(context->pInfo);
          context->levelUpSplash = LevelUpSplashCreate(context->board, context->pInfo);
          context->menuFocus = LEVEL_UP;
          context->controlEnabled = true;
          for (int i = 0; i < context->numAllies; i++) {
            AllyStatusSetVisible(context->allyStatuses[i], false);
          }
          context->nameBox->_visible = false;
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
    battleBehaviorUpdateModifiersTimeLeft(context->modifiers);
    _disableTechsIfTooExpensive(context->allies[0]);
    _disableTechParticlesIfNecessary(context, 0);
    
    context->sceneStage = SS_MOVE_FORWARD;
    context->currentActionIndex = 0;
    context->controlEnabled = true;
    COIMenuSetVisible(context->actionMenu);
    context->nameBox->_visible = true;
    context->movementOffset = 0;
    context->pointer->_visible = true;
    _focusActionMenu(context);
  } else {
    BattleAction action = context->actions[context->currentActionIndex];
    switch (context->sceneStage) {
    case SS_MOVE_FORWARD:
      // Actor may die before it's able to take its turn. Move to next action
      if (actorIsDead(action.actor)) {
        context->sceneStage = SS_MOVE_FORWARD;
        context->currentActionIndex++;
        return BR_CONTINUE;
      } else if (_moveActorForward(context, action.actor)) {
	      context->sceneStage = SS_TEXT;
        COISoundPlay(COI_SOUND_HIT);
      }
      break;
    case SS_TEXT:
      if (!context->summary) {
        // We're working with the target's sprite in this section
        action.target->sprite->_autoHandle = false;
        // Create ActionSummary. This holds the COIStrings
        // that describe the current action.
        context->descBox->_visible = true;
        context->summary = battleBehaviorDoAction(&context->actions[context->currentActionIndex], context->textType, context->board, context->descBox, context->pInfo, context->modifiers);
        // If it's a item, remove it from backpack on use
        // if (action.type == ITEM) {
        //   inventoryRemoveBackpackItemFirstInstance(context->pInfo->inventory,
        //   ItemListGetItem(context->pInfo->inventory->items, action.index));
        // }
      } else if (context->summary->finished) {
        ActionSummaryDestroy(context->summary, context->board);
        context->summary = NULL;
        context->sceneStage = SS_MOVE_BACKWARDS;
        context->descBox->_visible = false;
        // We're done with the target's sprite
        if (actorIsDead(action.target)) {
          action.target->sprite->_visible = false;
        } else {
          action.target->sprite->_autoHandle = true;
        }
        for (int i = 0; i < action.numOtherTargets; i++) {
          if (actorIsDead(action.otherTargets[i])) {
            action.otherTargets[i]->sprite->_visible = false;
          } else {
            action.otherTargets[i]->sprite->_autoHandle = true;
          }
        }
      } else {
        ActionSummaryAdvance(context->summary, selection);
        // Flicker effect on target actor and other targets
        if (context->summary->currentString > 0) {
          action.target->sprite->_visible = true;
          for (int i = 0; i < action.numOtherTargets; i++) {
            if (!actorIsDead(action.otherTargets[i])) {
              action.otherTargets[i]->sprite->_visible = true;
            }
          }
        }
        else if (context->summary->ticks % 10 == 0) {
          action.target->sprite->_visible = !action.target->sprite->_visible;
          for (int i = 0; i < action.numOtherTargets; i++) {
            if (!actorIsDead(action.otherTargets[i])) {
              action.otherTargets[i]->sprite->_autoHandle = false;
              action.otherTargets[i]->sprite->_visible = !action.otherTargets[i]->sprite->_visible;
            }
          }
        }
      }
      break;
    case SS_MOVE_BACKWARDS:
      if (_moveActorBackwards(context, action.actor)) {
        actorStandStill(action.actor);
        context->sceneStage = SS_MOVE_FORWARD;
        if (action.successfulFlee) {
          _disableAllTechs(context->allies[0]);
          return BR_FLEE;
        }
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
  // LinkedListResetCursor(board->dynamicSprites);
  // COISprite* sprite = (COISprite*)LinkedListNext(board->dynamicSprites);
  // while (sprite) {
  //   COIBoardRemoveDynamicSprite(board, sprite);
  //   COISpriteDestroy(sprite);
  //   sprite = (COISprite*)LinkedListNext(board->dynamicSprites);
  // }
  for (int i = 0; i < context->numAllies; i++) {
    // COIBoardRemoveDynamicSprite(board, context->techParticles[i]);
    // COISpriteDestroy(context->techParticles[i]);
    AllyStatusDestroy(context->allyStatuses[i]);
  }
  if (context->levelUpSplash) {
    LevelUpSplashDestroy(context->levelUpSplash, board);
  }
  battleBehaviorsDestroyModifiers(context->modifiers);
  free(context->techParticles);
  free(context->allyStatuses);
  free(context->enemies);
  free(context->actions);
  free(context);
  COIBoardDestroy(board);
}
