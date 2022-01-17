#include "inputloops.h"

bool testForCollision(COIBoard* board, COISprite* player, int changeX, int changeY) {
  int maxSpriteIndex = board->_spriteCount - 1;
  COISprite* currentSprite;
  int i;
  for (i = 0; i < maxSpriteIndex; i++) {
    currentSprite = board->_sprites[i];
    if (COISpriteCollision(currentSprite,
			  player->_x + changeX,
			  player->_y + changeY,
			  player->_width,
			  player->_height)) {
	return true;
      }
  }

  return false;
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

  switch (*direction) {
    case MOVING_LEFT:
      if (testForCollision(board, player, -5, 0)) {
	break;
      }
      COIBoardMoveSprite(board, player, -5, 0);
      playerCenterX = player->_x - board->_frameX + (player->_width / 2);
      if (playerCenterX <= board->_frameWidth / 2) {
        COIBoardShiftFrameX(board, -5);
      }
      break;
    case MOVING_RIGHT:
      if (testForCollision(board, player, 5, 0)) {
	break;
      }
      COIBoardMoveSprite(board, player, 5, 0);
      playerCenterX = player->_x - board->_frameX + (player->_width / 2);
      if (playerCenterX >= board->_frameWidth / 2) {
        COIBoardShiftFrameX(board, 5);
      }
      break;
    case MOVING_UP:
      if (testForCollision(board, player, 0, -5)) {
	break;
      }
      COIBoardMoveSprite(board, player, 0, -5);
      playerCenterY = player->_y - board->_frameY + (player->_height / 2);
      if (playerCenterY <= board->_frameHeight / 2) {
        COIBoardShiftFrameY(board, -5);
      }
      break;
    case MOVING_DOWN:
      if (testForCollision(board, player, 0, 5)) {
	break;
      }
      COIBoardMoveSprite(board, player, 0, 5);
      playerCenterY = player->_y - board->_frameY + (player->_height / 2);
      if (playerCenterY >= board->_frameHeight / 2) {
        COIBoardShiftFrameY(board, 5);
      }
  }
}
