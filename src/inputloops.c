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

void battle(COIBoard* board, SDL_Event* event, void* context) {
  switch (event->type) {
   
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
	  break;
        case SDLK_DOWN:
	  COIMenuIncrement(focusedMenu, 1);
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
    //COIMenuIncrement(armoryContext->menu, -2);
    
    // Re-adjust player sprite in threadtown and change to threadtown
    COISprite* player = threadTownBoard->_sprites[threadTownBoard->_spriteCount - 1];
    COIBoardMoveSprite(threadTownBoard, player, 0, 30);
    COIWindowSetBoard(window, threadTownBoard, &threadTown);
    return;
  }
}

void threadTown(COIBoard* board, SDL_Event* event, void* context) {
  COISprite* player = board->_sprites[board->_spriteCount - 1];
  int* direction = (int*) context;

  int playerCenterX, playerCenterY;
  switch (event->type) {
    case SDL_KEYDOWN:
      switch (event->key.keysym.sym) {
        case SDLK_LEFT:
          *direction = MOVING_LEFT;
          break;
        case SDLK_RIGHT:
          *direction = MOVING_RIGHT;
          break;
        case SDLK_UP:
          *direction = MOVING_UP;
          break;
        case SDLK_DOWN:
          *direction = MOVING_DOWN;
      }
      break;
    case SDL_KEYUP:
      if ((event->key.keysym.sym == SDLK_LEFT && *direction == MOVING_LEFT) ||
          (event->key.keysym.sym == SDLK_RIGHT && *direction == MOVING_RIGHT) ||
          (event->key.keysym.sym == SDLK_UP && *direction == MOVING_UP) ||
          (event->key.keysym.sym == SDLK_DOWN && *direction == MOVING_DOWN)) {
        *direction = MOVING_NONE;
      }
  }

  int collisionResult = -1;
  switch (*direction) {
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
    COIWindow* window;
  case ARMORY_DOOR:
    otherBoard = *(COIBoard**) (context + sizeof(int));
    window = *(COIWindow**) (context + sizeof(int) + sizeof(COIBoard*));
    COIWindowSetBoard(window, otherBoard, &armory);
    *direction = MOVING_NONE;
    break;
  case BATTLE:
    window = *(COIWindow**) (context + sizeof(int) + sizeof(COIBoard*));
    otherBoard = battleCreateBoard(window, board->loader, board);
    COIWindowSetBoard(window, otherBoard, &battle);
    *direction = MOVING_NONE;
    break;
  }
}
