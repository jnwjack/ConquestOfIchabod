#include "inputloops.h"

int testForCollision(COIBoard* board, COISprite* player, int changeX, int changeY) {
  int maxSpriteIndex = board->_spriteCount - 1;
  COISprite* currentSprite;
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

void armory(COIBoard* board, SDL_Event* event, void* context) {
  int* pointer = (int*) context;
  COISprite* pointerSprite = board->_sprites[board->_spriteCount - 1];
  bool selection = false;
  COIMenu** menuPtr = (context + sizeof(int) + sizeof(COIBoard*) + sizeof(COIWindow*));
  COIMenu* menu = *(menuPtr);
  switch (event->type) {
    case SDL_KEYDOWN:
      switch (event->key.keysym.sym) {
        case SDLK_UP:
	  COIMenuIncrement(menu, -1);
	  //menu->_current = (menu->_current - 1) < 0 ? 2 : (menu->_current - 1);
	  break;
        case SDLK_DOWN:
	  COIMenuIncrement(menu, 1);
	  //menu->_current = (menu->_current + 1) % menu->_visibleTextCount;
	  break;
        case SDLK_SPACE:
	  selection = true;
      }
  }

  if (selection && menu->_current == 2) {
    COIBoard* threadTownBoard = *(COIBoard**) (context + sizeof(int));
    COIWindow* window = *(COIWindow**) (context + sizeof(int) + sizeof(COIBoard*));
    
    // Re-adjust player sprite in threadtown
    COISprite* player = threadTownBoard->_sprites[threadTownBoard->_spriteCount - 1];
    COIBoardMoveSprite(threadTownBoard, player, 0, 30);
    COIWindowSetBoard(window, threadTownBoard, &threadTown);
    return;
  }
  board->_shouldDraw = true;
  //printf("pointerSprite in inputloop:%i\n", pointerSprite->_y);
  //int newPointerY = 50 + 40 * menu->_current;
  //COIBoardMoveSprite(board, pointerSprite, 0, newPointerY - pointerSprite->_y);
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
    COIBoard* board;
    COIWindow* window;
    case ARMORY_DOOR:
      board = *(COIBoard**) (context + sizeof(int));
      window = *(COIWindow**) (context + sizeof(int) + sizeof(COIBoard*));
      COIWindowSetBoard(window, board, &armory);
      *direction = MOVING_NONE;
      
      break;
  }
}
