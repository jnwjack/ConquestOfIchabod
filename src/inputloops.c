#include "inputloops.h"

int testForCollision(COIBoard* board, COISprite* player, int changeX, int changeY) {
  int maxSpriteIndex = board->_spriteCount - 1;
  COISprite* currentSprite = NULL;
  int i;
  int collisionResult;
  for (i = 0; i < maxSpriteIndex; i++) {
    currentSprite = board->_sprites[i];
    collisionResult = COISpriteCollision(currentSprite,
					 player->_x + changeX,
					 player->_y + changeY,
					 player->_width,
					 player->_height);
    if (collisionResult != COI_NO_COLLISION) {
      return collisionResult;
    }
  }

  return COI_NO_COLLISION;
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

void battle(COIBoard* board, SDL_Event* event, void* context) {
  BattleContext* battleContext = (BattleContext*)context;

  bool shouldExit = false;
  if (!battleContext->controlEnabled) {
    shouldExit = battleAdvanceScene(battleContext);
    if (shouldExit) {
      COIWindowSetBoard(battleContext->window, battleContext->outside, battleContext->outsideLoop);
      battleDestroyBoard(board);
    } else {
      board->_shouldDraw = true;
    }
    return;
  }

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
      shouldExit = battleHandleActionSelection(battleContext);
      break;
    case SUB_MENU:
      battleHandleSubMenuSelection(battleContext);
      break;
    case ACTORS:
      battleHandleActorSelect(battleContext);
      break;
    }

  }
  if (shouldExit) {
    COIWindowSetBoard(battleContext->window, battleContext->outside, battleContext->outsideLoop);
    battleDestroyBoard(board);
  }
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
    
    // Re-adjust player sprite in threadtown and change to threadtown
    COISprite* player = threadTownBoard->_sprites[threadTownBoard->_spriteCount - 1];
    COIBoardMoveSprite(threadTownBoard, player, 0, 30);
    COIWindowSetBoard(window, threadTownBoard, &threadTown);

    armoryDestroy(armoryContext);
    COIBoardDestroy(board);
    return;
  }
}

void threadTown(COIBoard* board, SDL_Event* event, void* context) {
  COISprite* player = board->_sprites[board->_spriteCount - 1];
  TownContext* townContext = (TownContext*)context;
  //int* direction = (int*) context;

  int playerCenterX, playerCenterY;
  switch (event->type) {
    case SDL_KEYDOWN:
      switch (event->key.keysym.sym) {
        case SDLK_LEFT:
	  townContext->direction = MOVING_LEFT;
          break;
        case SDLK_RIGHT:
	  townContext->direction = MOVING_RIGHT;
          break;
        case SDLK_UP:
	  townContext->direction = MOVING_UP;
          break;
        case SDLK_DOWN:
	  townContext->direction = MOVING_DOWN;
	  break;
      }
      break;
    case SDL_KEYUP:
      if ((event->key.keysym.sym == SDLK_LEFT && townContext->direction == MOVING_LEFT) ||
          (event->key.keysym.sym == SDLK_RIGHT && townContext->direction == MOVING_RIGHT) ||
          (event->key.keysym.sym == SDLK_UP && townContext->direction == MOVING_UP) ||
          (event->key.keysym.sym == SDLK_DOWN && townContext->direction == MOVING_DOWN)) {
	townContext->direction = MOVING_NONE;
      }
  }

  int collisionResult = -1;
  switch (townContext->direction) {
    case MOVING_LEFT:
      collisionResult = testForCollision(board, player, -5, 0);
      if (collisionResult) {
	break;
      }
      COIBoardMoveSprite(board, player, -5, 0);
      playerCenterX = player->_x - board->_frameX + (player->_width / 2);
      if (playerCenterX <= board->_frameWidth / 2) {
        COIBoardShiftFrameX(board, -5);
      }
      return;
    case MOVING_RIGHT:
      collisionResult = testForCollision(board, player, 5, 0);
      if (collisionResult) {
	break;
      }
      COIBoardMoveSprite(board, player, 5, 0);
      playerCenterX = player->_x - board->_frameX + (player->_width / 2);
      if (playerCenterX >= board->_frameWidth / 2) {
        COIBoardShiftFrameX(board, 5);
      }
      return;
    case MOVING_UP:
      collisionResult = testForCollision(board, player, 0, -5);
      if (collisionResult) {
	break;
      }
      COIBoardMoveSprite(board, player, 0, -5);
      playerCenterY = player->_y - board->_frameY + (player->_height / 2);
      if (playerCenterY <= board->_frameHeight / 2) {
        COIBoardShiftFrameY(board, -5);
      }
      return;
    case MOVING_DOWN:
      collisionResult = testForCollision(board, player, 0, 5);
      if (collisionResult) {
	break;
      }
      COIBoardMoveSprite(board, player, 0, 5);
      playerCenterY = player->_y - board->_frameY + (player->_height / 2);
      if (playerCenterY >= board->_frameHeight / 2) {
        COIBoardShiftFrameY(board, 5);
      }
      return;
  }
  switch (collisionResult) {
    COIBoard* otherBoard;
  case ARMORY_DOOR:
    otherBoard = armoryCreateBoard(townContext->window, board->loader, board, townContext->pInfo->inventory);
    COIWindowSetBoard(townContext->window, otherBoard, &armory);
    townContext->direction = MOVING_NONE;
    break;
  case BATTLE:
    otherBoard = battleCreateBoard(townContext->window, board->loader, board, threadTown, ACTOR_SKELETON, townContext->pInfo);
    COIWindowSetBoard(townContext->window, otherBoard, &battle);
    // For now, move sprite so we don't instantly go back into battle.
    // In the future, the sprite we run into will disappear after battle.
    COIBoardMoveSprite(board, player, 120, 0);
    townContext->direction = MOVING_NONE;
    break;
  }
}
