// Should have a better system for includes
#include "engine/COIWindow.h"
#include "engine/COIBoard.h"

#define MOVING_NONE 0
#define MOVING_LEFT 1
#define MOVING_RIGHT 2
#define MOVING_UP 3
#define MOVING_DOWN 4

void testCallback(COIBoard* board, SDL_Event* event, void* context) {
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
      // case SDLK_LEFT:
      // case SDLK_RIGHT:
      // case SDLK_UP:
      // case SDLK_DOWN:
      //   *direction = MOVING_NONE;
  }

  switch (*direction) {
    case MOVING_LEFT:
      COIBoardMoveSprite(board, player, -5, 0);
      playerCenterX = player->_x - board->_frameX + (player->_width / 2);
      if (playerCenterX <= board->_frameWidth / 2) {
        COIBoardShiftFrameX(board, -5);
      }
      break;
    case MOVING_RIGHT:
      COIBoardMoveSprite(board, player, 5, 0);
      playerCenterX = player->_x - board->_frameX + (player->_width / 2);
      if (playerCenterX >= board->_frameWidth / 2) {
        COIBoardShiftFrameX(board, 5);
      }
      break;
    case MOVING_UP:
      COIBoardMoveSprite(board, player, 0, -5);
      playerCenterY = player->_y - board->_frameY + (player->_height / 2);
      if (playerCenterY <= board->_frameHeight / 2) {
        COIBoardShiftFrameY(board, -5);
      }
      break;
    case MOVING_DOWN:
      COIBoardMoveSprite(board, player, 0, 5);
      playerCenterY = player->_y - board->_frameY + (player->_height / 2);
      if (playerCenterY >= board->_frameHeight / 2) {
        COIBoardShiftFrameY(board, 5);
      }
  }
}


int main(int argc, char** argv) {
  COIAssetLoader* loader = COIAssetLoaderCreate("src/engine/etc/mapping.cfg");
  COILoop loop = &testCallback;
  int* context = malloc(sizeof(int));

  COIWindow* window = COIWindowCreate();
  COIBoard* board = COIBoardCreate(2, 132, 28, 255, 700, 700);  

  COIBoardLoadSpriteMap(board, loader, COIWindowGetRenderer(window), "src/spritemaps/threadtown.dat");
  COIWindowSetLoop(window, loop, (void*)context);
  COIWindowSetBoard(window, board);
  COIWindowLoop(window);

  free(context);
  COIAssetLoaderDestroy(loader);
  COIBoardDestroy(board);
  COIWindowDestroy(window);

  return 0;
}
