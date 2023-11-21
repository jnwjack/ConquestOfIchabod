#include "GameOver.h"

COIBoard* gameOverCreateBoard(COIWindow* window, COIAssetLoader* loader) {
  COIBoard* board = COIBoardCreate(20, 20, 20, 225, 640, 480, loader);
  
  COITextType* textType = COITextTypeCreate(40, 255, 255, 255, COIWindowGetRenderer(window));

  // Replace with 640 x 480 COISprite that displays "game over" image.
  COIString* gameOverString = COIStringCreate("Game Over", 220, 200, textType);
  COIBoardAddString(board, gameOverString);

  return board;
}
