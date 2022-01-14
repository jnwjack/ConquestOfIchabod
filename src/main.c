// Should have a better system for includes
#include "engine/COIWindow.h"
#include "engine/COIBoard.h"

void testCallback(COIBoard* board, SDL_Keycode key) {
  COISprite* player = board->_sprites[board->_spriteCount - 1];

  int playerCenterX, playerCenterY;
  
  switch (key) {
    case SDLK_LEFT:
      COIBoardMoveSprite(board, player, -5, 0);
      playerCenterX = player->_x - board->_frameX + (player->_width / 2);
      if (playerCenterX <= board->_frameWidth / 2) {
	COIBoardShiftFrameX(board, -5);
      }
      break;
    case SDLK_RIGHT:
      COIBoardMoveSprite(board, player, 5, 0);
      playerCenterX = player->_x - board->_frameX + (player->_width / 2);
      if (playerCenterX >= board->_frameWidth / 2) {
	COIBoardShiftFrameX(board, 5);
      }
      break;
    case SDLK_UP:
      COIBoardMoveSprite(board, player, 0, -5);
      playerCenterY = player->_y - board->_frameY + (player->_height / 2);
      if (playerCenterY <= board->_frameHeight / 2) {
	COIBoardShiftFrameY(board, -5);
      }
      break;
    case SDLK_DOWN:
      COIBoardMoveSprite(board, player, 0, 5);
      playerCenterY = player->_y - board->_frameY + (player->_height / 2);
      if (playerCenterY >= board->_frameHeight / 2) {
	COIBoardShiftFrameY(board, 5);
      }
      break;
  }
}


int main(int argc, char** argv) {
  COIAssetLoader* loader = COIAssetLoaderCreate("src/engine/etc/mapping.cfg");
  COILoop loop = &testCallback;

  COIWindow* window = COIWindowCreate();
  COIBoard* board = COIBoardCreate(2, 132, 28, 255, 700, 700);  

  COIBoardLoadSpriteMap(board, loader, COIWindowGetRenderer(window), "src/spritemaps/turtles.dat");
  COIWindowSetLoop(window, loop);
  COIWindowSetBoard(window, board);
  COIWindowLoop(window);

  COIAssetLoaderDestroy(loader);
  COIBoardDestroy(board);
  COIWindowDestroy(window);

  return 0;
}
