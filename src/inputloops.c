#include "inputloops.h"


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

void _processBattleResult(COIBoard* board, BattleContext* battleContext, BattleResult result) {
  COIBoard* nextBoard = NULL;
  switch (result) {
  case BR_LOSS:
    nextBoard = gameOverCreateBoard(battleContext->window, board->loader);
    COIWindowSetBoard(battleContext->window, nextBoard, NULL);
    break;
  case BR_FLEE:
  case BR_WIN:
    //COIBoardMoveSprite(board, battleContext->allies[0]->sprite, context->playerOutsideX + 120, );
    COISpriteSetPos(battleContext->allies[0]->sprite, battleContext->playerOutsideX, battleContext->playerOutsideY);
    COIWindowSetBoard(battleContext->window, battleContext->outside, battleContext->outsideLoop);
    battleDestroyBoard(board);
    break;
  default:
    board->_shouldDraw = true;
  }
}

void battle(COIBoard* board, SDL_Event* event, void* context) {
  BattleContext* battleContext = (BattleContext*)context;

  // Behavior when not accepting user input
  BattleResult result = BR_CONTINUE;
  if (!battleContext->controlEnabled) {
    result = battleAdvanceScene(battleContext);
    _processBattleResult(board, battleContext, result);
    return;
  }

  // Behavior when accepting user input
  bool selection = false;
  COIMenu* menu = battleContext->actionMenu;
  switch (event->type) {
    case SDL_KEYDOWN:
      if (event->key.keysym.sym == SDLK_LEFT) {
	battleHandleBack(battleContext);
      }else {
	if (battleContext->menuFocus == ACTION_MENU) {
	  selection = handleMenuInput(battleContext->actionMenu, event);
	} else if (battleContext->menuFocus == SUB_MENU) {
	  selection = handleMenuInput(battleContext->subMenu, event);
	} else {
	  switch (event->key.keysym.sym) {
	  case SDLK_UP:
	    battleMovePointer(battleContext, -1);
	    break;
	  case SDLK_DOWN:
	    battleMovePointer(battleContext, 1);
	    break;
	  case SDLK_SPACE:
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
    }
  }
  _processBattleResult(board, battleContext, result);
}

void armory(COIBoard* board, SDL_Event* event, void* context) {
  ArmoryContext* armoryContext = (ArmoryContext*)context;

  bool selection = false;
  bool back = false;
  COIMenu* focusedMenu = armoryContext->confirmActive ? armoryContext->confirmMenu : armoryContext->currentMenu;
  switch (event->type) {
    case SDL_KEYDOWN:
      switch (event->key.keysym.sym) {
        case SDLK_UP:
	  COIMenuIncrement(focusedMenu, -1);
	  COIMenuSetVisible(focusedMenu);
	  break;
        case SDLK_DOWN:
	  COIMenuIncrement(focusedMenu, 1);
	  COIMenuSetVisible(focusedMenu);
	  break;
        case SDLK_SPACE:
	  selection = true;
	  break;
        case SDLK_LEFT:
	  back = true;
	  break;
      }
      break;
    default:
      return;
  }

  board->_shouldDraw = true;

  // Return to first menu
  if (back && armoryContext->currentMenu != armoryContext->menu) {
    COIMenuSetInvisible(armoryContext->currentMenu);
    COIMenuReset(armoryContext->currentMenu);
    armoryContext->currentMenu = armoryContext->menu;
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
    COIWindowSetBoard(window, threadTownBoard, &threadTown);

    armoryDestroy(armoryContext);
    COIBoardDestroy(board);
    return;
  }
}

void threadTown(COIBoard* board, SDL_Event* event, void* context) {
  TownContext* townContext = (TownContext*)context;
  Actor* player = townContext->pInfo->party[0];

  switch (event->type) {
  case SDL_KEYDOWN:
    switch (event->key.keysym.sym) {
    case SDLK_LEFT:
      townProcessMovementInput(townContext, MOVING_LEFT);
      break;
    case SDLK_RIGHT:
      townProcessMovementInput(townContext, MOVING_RIGHT);
      break;
    case SDLK_UP:
      townProcessMovementInput(townContext, MOVING_UP);
      break;
    case SDLK_DOWN:
  
    townProcessMovementInput(townContext, MOVING_DOWN);
      break;
    }
    break;
  case SDL_KEYUP:
    if ((event->key.keysym.sym == SDLK_LEFT && player->nextMovementDirection == MOVING_LEFT) ||
	(event->key.keysym.sym == SDLK_RIGHT && player->nextMovementDirection == MOVING_RIGHT) ||
	(event->key.keysym.sym == SDLK_UP && player->nextMovementDirection == MOVING_UP) ||
	(event->key.keysym.sym == SDLK_DOWN && player->nextMovementDirection == MOVING_DOWN)) {
      townProcessMovementInput(townContext, MOVING_NONE);
      board->_shouldDraw = true;
    }
    break;
  }

  if (townShouldCheckForCollision(context)) {
    int collision = townCheckForCollision(context);
    // Town context handle generic collision, moving actors around, etc.
    townProcessCollisionType(townContext, collision);
    // Input loop handles collisions that lead to changes in the input loop (going into a shop, etc.)

    COIBoard* otherBoard;
    switch (collision) {
    case ARMORY_DOOR:
      player->movementDirection = MOVING_NONE;
      otherBoard = armoryCreateBoard(townContext->window, board->loader, board, townContext->pInfo->inventory);
      COIWindowSetBoard(townContext->window, otherBoard, &armory);
      break;
    default:
      //townMovePlayer(townContext);
      break;
    }
  }
  townMovePlayer(townContext);
  
}


/*void threadTown(COIBoard* board, SDL_Event* event, void* context) {
  //COISprite* player = board->_sprites[board->_spriteCount - 1];
  TownContext* townContext = (TownContext*)context;
  //int* direction = (int*) context;
  Actor* player = townContext->pInfo->party[0];
  int playerCenterX, playerCenterY;
  switch (event->type) {
    case SDL_KEYDOWN:
      switch (event->key.keysym.sym) {
        case SDLK_LEFT:
	  actorStartMovement(player, MOVING_LEFT, OVERWORLD_MOVE_SPEED);
	  townContext->direction = MOVING_LEFT;
          break;
        case SDLK_RIGHT:
	  actorStartMovement(player, MOVING_RIGHT, OVERWORLD_MOVE_SPEED);
	  townContext->direction = MOVING_RIGHT;
          break;
        case SDLK_UP:
	  actorStartMovement(player, MOVING_UP, OVERWORLD_MOVE_SPEED);
	  townContext->direction = MOVING_UP;
          break;
        case SDLK_DOWN:
	  actorStartMovement(player, MOVING_DOWN, OVERWORLD_MOVE_SPEED);
	  townContext->direction = MOVING_DOWN;
	  break;
      }
      break;
    case SDL_KEYUP:
      if ((event->key.keysym.sym == SDLK_LEFT && townContext->direction == MOVING_LEFT) ||
          (event->key.keysym.sym == SDLK_RIGHT && townContext->direction == MOVING_RIGHT) ||
          (event->key.keysym.sym == SDLK_UP && townContext->direction == MOVING_UP) ||
          (event->key.keysym.sym == SDLK_DOWN && townContext->direction == MOVING_DOWN)) {
	actorQueueNextDirection(player, MOVING_NONE);
	townContext->direction = MOVING_NONE;
	//actorStandStill(player);
	board->_shouldDraw = true;
      }
      break;
  }

  int collisionResult = -1;
  //printf("movement: %i\n", player->movementDirection);
  switch (player->movementDirection) {
    case MOVING_LEFT:
      collisionResult = testForCollision(board, player->sprite, -1 * OVERWORLD_MOVE_SPEED, 0);
      if (collisionResult == COI_COLLISION) {
	actorStandStill(player);
	break;
      }
      //COIBoardMoveSprite(board, player, -5, 0);
      actorMove(player, -1 * OVERWORLD_MOVE_SPEED, 0, board);
      playerCenterX = player->sprite->_x - board->_frameX + (player->sprite->_width / 2);
      if (playerCenterX <= board->_frameWidth / 2) {
        COIBoardShiftFrameX(board, -1 * OVERWORLD_MOVE_SPEED);
      }
      break;
    case MOVING_RIGHT:
      collisionResult = testForCollision(board, player->sprite, OVERWORLD_MOVE_SPEED, 0);
      if (collisionResult == COI_COLLISION) {
	actorStandStill(player);
	break;
      }
      //COIBoardMoveSprite(board, player, 5, 0);
      actorMove(player, OVERWORLD_MOVE_SPEED, 0, board);
      playerCenterX = player->sprite->_x - board->_frameX + (player->sprite->_width / 2);
      if (playerCenterX >= board->_frameWidth / 2) {
        COIBoardShiftFrameX(board, OVERWORLD_MOVE_SPEED);
      }
      break;
    case MOVING_UP:
      collisionResult = testForCollision(board, player->sprite, 0, -1 * OVERWORLD_MOVE_SPEED);
      if (collisionResult == COI_COLLISION) {
	actorStandStill(player);
	break;
      }
      //COIBoardMoveSprite(board, player, 0, -5);
      actorMove(player, 0, -1 * OVERWORLD_MOVE_SPEED, board);
      playerCenterY = player->sprite->_y - board->_frameY + (player->sprite->_height / 2);
      if (playerCenterY <= board->_frameHeight / 2) {
        COIBoardShiftFrameY(board, -1 * OVERWORLD_MOVE_SPEED);
      }
      break;
    case MOVING_DOWN:
      collisionResult = testForCollision(board, player->sprite, 0, OVERWORLD_MOVE_SPEED);
      if (collisionResult == COI_COLLISION) {
	actorStandStill(player);
	break;
      }
      //COIBoardMoveSprite(board, player, 0, 5);
      actorMove(player, 0, OVERWORLD_MOVE_SPEED, board);
      playerCenterY = player->sprite->_y - board->_frameY + (player->sprite->_height / 2);
      if (playerCenterY >= board->_frameHeight / 2) {
        COIBoardShiftFrameY(board, OVERWORLD_MOVE_SPEED);
      }
      break;
  default:
    collisionResult = testForCollision(board, player->sprite, 0, 0);
  }

  COIBoard* otherBoard;
  switch (collisionResult) {
  case ARMORY_DOOR:
    otherBoard = armoryCreateBoard(townContext->window, board->loader, board, townContext->pInfo->inventory);
    COIWindowSetBoard(townContext->window, otherBoard, &armory);
    townContext->direction = MOVING_NONE;
    break;
  default:
    break;
  }

  townUpdateTerrain(townContext, collisionResult);

  if (townCheckForBattle(townContext)) {
    otherBoard = battleCreateBoard(townContext->window, board->loader, board, threadTown, ACTOR_SKELETON, townContext->pInfo);
    COIWindowSetBoard(townContext->window, otherBoard, &battle);
    // For now, move sprite so we don't instantly go back into battle.
    // In the future, the sprite we run into will disappear after battle.
    townContext->direction = MOVING_NONE;
    //BattleContext* bbContext = (BattleContext*)otherBoard->context;
  }
}
*/
