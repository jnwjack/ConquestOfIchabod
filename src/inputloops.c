#include "inputloops.h"
#include "tests.h"


// Can extend this to support joystick, etc. later.
static int _sdlEventToDirectionalInput(SDL_Event* event) {
  switch (event->key.keysym.sym) {
  case SDLK_UP:
  case SDLK_w:
    return MOVING_UP;
  case SDLK_DOWN:
  case SDLK_s:
    return MOVING_DOWN;
  case SDLK_LEFT:
  case SDLK_a:
    return MOVING_LEFT;
  case SDLK_RIGHT:
  case SDLK_d:
    return MOVING_RIGHT;
  case SDLK_z:
  case SDLK_SPACE:
    return MOVING_SELECT;
  case SDLK_x:
    return MOVING_DELETE;
  case SDLK_ESCAPE:
    return MOVING_PAUSE;
  default:
    return MOVING_NONE;
  }
}

static void _changeBoardToThreadTown(COIBoard* townBoard) {
  COIWindowSetBoard(COI_GLOBAL_WINDOW, townBoard, &threadTown);
  townApplyTimeChanges((TownContext*)townBoard->context);
}

bool handleMenuInput(COIMenu* menu, SDL_Event* event) {
  bool selection = false;
  switch (event->key.keysym.sym) {
    case SDLK_UP:
      COIMenuIncrement(menu, -1);
      COIMenuSetVisible(menu);
      break;
    case SDLK_DOWN:
      COIMenuIncrement(menu, 1);
      COIMenuSetVisible(menu);
      break;
    case SDLK_SPACE:
      selection = true;
      break;
  }

  return selection;
}

static void _processBattleResult(COIBoard* board, BattleContext* battleContext, BattleResult result) {
  COIBoard* nextBoard = NULL;
  switch (result) {
  case BR_LOSS:
    nextBoard = gameOverCreateBoard(battleContext->window, board->loader, GAME_OVER_DEATH, battleContext->pInfo);
    COIWindowSetBoard(battleContext->window, nextBoard, gameOver);
    break;
  case BR_FLEE:
  case BR_WIN:
    COISpriteSetPos(battleContext->allies[0]->sprite, battleContext->playerOutsideX, battleContext->playerOutsideY);
    _changeBoardToThreadTown(battleContext->outside); // Breaks if we have another "overworld" board besides the town.
    // COIWindowSetBoard(battleContext->window, battleContext->outside, battleContext->outsideLoop);
    TimeStateIncrement(1);
    playerCheckForEviction(battleContext->pInfo);
    battleDestroyBoard(board);
    break;
  default:
    board->_shouldDraw = true;
  }
}

void battle(COIBoard* board, SDL_Event* event, void* context) {
  // printf("before tick\n");
  BattleContext* battleContext = (BattleContext*)context;
  battleTick(battleContext);

  // Behavior when not accepting user input
  BattleResult result = BR_CONTINUE;
  if (!battleContext->controlEnabled) {
    // Can speed up animations by inputting a selection
    bool selection = _sdlEventToDirectionalInput(event) == MOVING_SELECT;
    // printf("before advance scene\n");
    result = battleAdvanceScene(battleContext, selection);
    _processBattleResult(board, battleContext, result);
    // printf("after advance scene\n");
    return;
  }

  // Behavior when accepting user input
  bool selection = false;
  COIMenu* menu = battleContext->actionMenu;
  switch (event->type) {
    case SDL_KEYDOWN:
      if (event->key.keysym.sym == SDLK_x && battleContext->menuFocus != LEVEL_UP) {
	      battleHandleBack(battleContext);
      } else {
        if (battleContext->menuFocus == ACTION_MENU) {
          selection = COIMenuHandleInput(battleContext->actionMenu, _sdlEventToDirectionalInput(event));
        } else if (battleContext->menuFocus == SUB_MENU) {
          selection = COIMenuHandleInput(battleContext->subMenu, _sdlEventToDirectionalInput(event));
        } else if (battleContext->menuFocus == LEVEL_UP) {
          selection = LevelUpSplashProcessInput(battleContext->levelUpSplash, _sdlEventToDirectionalInput(event));
        } else {
	        switch (event->key.keysym.sym) {
	        case SDLK_UP:
            battleMovePointer(battleContext, MOVING_UP);
            break;
          case SDLK_DOWN:
            battleMovePointer(battleContext, MOVING_DOWN);
            break;
          case SDLK_LEFT:
            battleMovePointer(battleContext, MOVING_LEFT);
            break;
          case SDLK_RIGHT:
            battleMovePointer(battleContext, MOVING_RIGHT);
            break;    
          case SDLK_z:
            selection = true;
            break;
	        }
	      }
      }
      break;
    default:
      return;
  }

  board->_shouldDraw = true;

  if (selection) {
    switch (battleContext->menuFocus) {
    case ACTION_MENU:
      result = battleHandleActionSelection(battleContext);
      break;
    case SUB_MENU:
      battleHandleSubMenuSelection(battleContext);
      break;
    case ACTORS:
      battleHandleActorSelect(battleContext);
      break;
    case LEVEL_UP:
      if (LevelUpSplashProcessSelection(battleContext->levelUpSplash, battleContext->pInfo)) {
        result = BR_WIN;
      }
      break;
    }
  }
  _processBattleResult(board, battleContext, result);
}

void title(COIBoard* board, SDL_Event* event, void* context) {
  TitleContext* titleContext = (TitleContext*)context;

  titleTick(titleContext);

  if (event->type == SDL_KEYDOWN) {
    titleProcessInput(titleContext, _sdlEventToDirectionalInput(event));
  }

  TitleNextBoard nextBoard = titleGetNextBoard(titleContext);
  if (nextBoard == TITLE_NEW_GAME) {
    // Global item data
   ItemList* itemList = loadItems();

   // Test inventory
   Inventory* inventory = createTestInventory(itemList);

    // Initialize player data
    COISprite* playerSprite = COISpriteCreateFromAssetID(2240, 1984, 32, 32, COI_GLOBAL_LOADER, 1, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
    PlayerInfo* pInfo = playerInfoCreate(titleContext->kb.name, playerSprite, inventory, titleContext->cs.currentClass); // jnw cleanup: leaks
    COIBoard* townBoard = townCreateBoard(COI_GLOBAL_WINDOW, COI_GLOBAL_LOADER, pInfo);
    _changeBoardToThreadTown(townBoard);
    titleDestroyBoard(titleContext);
  } else if (nextBoard == TITLE_CONTINUE_GAME) {
    // Global item data
    ItemList* itemList = loadItems();

    // Test inventory
    Inventory* inventory = createTestInventory(itemList);

    // Initialize player data
    COISprite* playerSprite = COISpriteCreateFromAssetID(2240, 1984, 32, 32, COI_GLOBAL_LOADER, 1, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
    PlayerInfo* pInfo = playerDecode(itemList, playerSprite, inventory);
    COIBoard* townBoard = townCreateBoard(COI_GLOBAL_WINDOW, COI_GLOBAL_LOADER, pInfo);
    _changeBoardToThreadTown(townBoard);
    titleDestroyBoard(titleContext);
  }
}

void armory(COIBoard* board, SDL_Event* event, void* context) {
  ArmoryContext* armoryContext = (ArmoryContext*)context;

  bool selection = false;
  bool back = false;
  COIMenu* focusedMenu = armoryContext->confirmActive ? armoryContext->confirmMenu : armoryContext->currentMenu;
  switch (event->type) {
  case SDL_KEYDOWN:
  {
    int move = _sdlEventToDirectionalInput(event);
    switch (move) {
    case MOVING_UP:
      // COISoundPlay(COI_SOUND_BLIP);
      COIMenuIncrement(focusedMenu, -1);
      COIMenuSetVisible(focusedMenu);
      break;
    case MOVING_DOWN:
      // COISoundPlay(COI_SOUND_BLIP);
      COIMenuIncrement(focusedMenu, 1);
      COIMenuSetVisible(focusedMenu);
	    break;
    case MOVING_SELECT:
      COISoundPlay(COI_SOUND_SELECT); // Should move this event processing to the COIMenuHandleInput function.
	    selection = true;
	    break;
    case MOVING_DELETE:
      COISoundPlay(COI_SOUND_SELECT);
	    back = true;
	    break;
    }
    break;
  }
  default:
    return;
  }

  board->_shouldDraw = true;

  // Return to first menu
  if (back && armoryContext->currentMenu != armoryContext->menu) {
    if (armoryContext->confirmActive) {
      armoryDisableConfirmMenu(armoryContext);
    } else {
      COIMenuSetInvisible(armoryContext->currentMenu);
      COIMenuReset(armoryContext->currentMenu);
      armoryContext->currentMenu = armoryContext->menu;
    }
    return;
  }

  if (!selection) {
    return;
  }

  // If confirm menu active
  if (armoryContext->confirmActive) {
    switch (armoryContext->confirmMenu->_current) {
    case 0:
      armoryDisableConfirmMenu(armoryContext);
      break;
    case 1:
      if (armoryContext->currentMenu == armoryContext->buyMenu) {
	      armoryBuyItem(board);
      } else {
	      armorySellItem(board);
      }
      break;
    }
    return;
  }

  // Behavior for buy and sell menus
  if (armoryContext->currentMenu != armoryContext->menu) {
    armoryEnableConfirmMenu(armoryContext);
  }

  // Buy
  if (armoryContext->currentMenu == armoryContext->menu && armoryContext->currentMenu->_current == 0) {
    COIMenuSetVisible(armoryContext->buyMenu);
    armoryContext->currentMenu = armoryContext->buyMenu;
  }

  // Sell
  if (armoryContext->currentMenu == armoryContext->menu && armoryContext->currentMenu->_current == 1) {
    COIMenuSetVisible(armoryContext->sellMenu);
    armoryContext->currentMenu = armoryContext->sellMenu;
  }
  
  // Exit
  if (armoryContext->currentMenu == armoryContext->menu && armoryContext->currentMenu->_current == 2) {
    COIBoard* threadTownBoard = armoryContext->outsideBoard;
    COIWindow* window = armoryContext->window;

    // Reset menus and pointer
    COIMenuReset(armoryContext->buyMenu);
    COIMenuReset(armoryContext->menu);
    COIMenuSetInvisible(armoryContext->buyMenu);
    armoryContext->currentMenu = armoryContext->menu;
    _changeBoardToThreadTown(threadTownBoard);

    armoryDestroy(armoryContext);
    COIBoardDestroy(board);
    return;
  }
}

void gameOver(COIBoard* board, SDL_Event* event, void* context) {
  GameOverContext* gameOverContext = (GameOverContext*)context;
  switch (event->type) {
  case SDL_KEYDOWN:
    switch(event->key.keysym.sym) {
    case SDLK_SPACE:
    case SDLK_ESCAPE:
      COIWindowSetBoard(COI_GLOBAL_WINDOW, titleCreateBoard(), title);
      gameOverDestroyBoard(gameOverContext);
      break;
    }
    default:
      return;
  }
}

void threadTown(COIBoard* board, SDL_Event* event, void* context) {
  TownContext* townContext = (TownContext*)context;
  Actor* player = townContext->pInfo->party[0];

  if (!TimeStateInFuture(&END_TIME)) {
    COIBoard* gameOverBoard;
    if (townContext->pInfo->working) {
      gameOverBoard = gameOverCreateBoard(COI_GLOBAL_WINDOW, COI_GLOBAL_LOADER, GAME_OVER_TIME_AND_JOB, townContext->pInfo);
    } else {
      gameOverBoard = gameOverCreateBoard(COI_GLOBAL_WINDOW, COI_GLOBAL_LOADER, GAME_OVER_TIME, townContext->pInfo);
    }
    COIWindowSetBoard(COI_GLOBAL_WINDOW, gameOverBoard, gameOver);
    return;
  }

  if (townContext->willEnterBattle) {
    COIBoard* battleBoard = battleCreateBoard(townContext->window,
					      board->loader,
					      board,
					      threadTown,
					      townContext->terrain,
					      townContext->pInfo);
    COIWindowSetBoard(townContext->window, battleBoard, &battle);
    townContext->willEnterBattle = false;
    return;
  }
  
  switch (event->type) {
  case SDL_KEYDOWN:
  {
    int input = _sdlEventToDirectionalInput(event);
    switch (input) {
    case MOVING_PAUSE:
      townTogglePauseOverlay(townContext);
      break;
    case MOVING_SELECT:
      townProcessSelectionInput(townContext);
      break;
    default:
      townProcessDirectionalInput(townContext, input);
      break;
    }
    break;
  }
  case SDL_KEYUP:
  {
    int input = _sdlEventToDirectionalInput(event);
    if ((input == MOVING_LEFT && player->nextMovementDirection == MOVING_LEFT) ||
      (input == MOVING_RIGHT && player->nextMovementDirection == MOVING_RIGHT) ||
      (input == MOVING_UP && player->nextMovementDirection == MOVING_UP) ||
      (input == MOVING_DOWN && player->nextMovementDirection == MOVING_DOWN)) {
      townProcessDirectionalInput(townContext, MOVING_NONE);
      board->_shouldDraw = true;
    }
    break;
  }
  }

  if (townShouldCheckForCollision(context)) {
    int collision = townCheckForCollision(context, player);
    // Town context handle generic collision, moving actors around, etc.
    townProcessCollisionType(townContext, collision);
    // Input loop handles collisions that lead to changes in the input loop (going into a shop, etc.)
    COIBoard* otherBoard;
    switch (collision) {
    case SHOP_DOOR:
      player->movementDirection = MOVING_NONE;
      //otherBoard = armoryCreateBoard(townContext->window, board->loader, board, townContext->pInfo->inventory);
      otherBoard = armoryCreateBoardForGeneralStore(board, townContext->pInfo->inventory);
      COIWindowSetBoard(townContext->window, otherBoard, &armory);
      break;      
    case ARMORY_DOOR:
      player->movementDirection = MOVING_NONE;
      //otherBoard = armoryCreateBoard(townContext->window, board->loader, board, townContext->pInfo->inventory);
      otherBoard = armoryCreateBoardForWeaponsStore(board, townContext->pInfo->inventory);
      COIWindowSetBoard(townContext->window, otherBoard, &armory);
      break;
    case RENTABLE_HOUSE_DOOR:
      player->movementDirection = MOVING_NONE;
      if (townContext->pInfo->renting == RS_RENTING) {
        townContext->pauseOverlay->dirty = true;
        otherBoard = RentHouseCreateBoard(townContext->pInfo, board);
        COIWindowSetBoard(COI_GLOBAL_WINDOW, otherBoard, &rentHouse);
      }
      break;
    default:
      //townMovePlayer(townContext);
      break;
    }
  }
  
  townMovePlayer(townContext);
  
  // Handle other tasks (generate NPC movements, animating text)
  townTick(townContext);
  
  townMoveNPCs(townContext);
}

void rentHouse(COIBoard* board, SDL_Event* event, void* context) {
  RentHouseContext* rhContext = (RentHouseContext*)context;

  bool shouldExit = false;
  switch (event->type) {
  case SDL_KEYDOWN:
  {
    int input = _sdlEventToDirectionalInput(event);
    switch (input) {
    case MOVING_SELECT:
      shouldExit = RentHouseProcessSelectionInput(rhContext);
      break;
    case MOVING_LEFT:
    case MOVING_RIGHT:
    case MOVING_UP:
    case MOVING_DOWN:
      RentHouseProcessDirectionalInput(rhContext,
				       input);
      break;
    }
  }
  }

  RentHouseTick(rhContext);

  if (shouldExit) {
    _changeBoardToThreadTown(rhContext->outsideBoard);
    RentHouseDestroyBoard(rhContext);
  }
}

