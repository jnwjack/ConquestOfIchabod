#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "engine/COIWindow.h"
#include "engine/COIBoard.h"

int main(int argc, char** argv) {
  COIWindow* window = COIWindowCreate();
  COIBoard* board = COIBoardCreate(230, 25, 25, 255);
  COIWindowSetBoard(window, board);
  COIWindowLoop(window);
  COIWindowDestroy(window);

  return 0;
}
