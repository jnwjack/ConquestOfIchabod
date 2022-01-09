#include "COIWindow.h"

COIWindow* COIWindowCreate() {
  // Initialize SDL
  SDL_Init(SDL_INIT_VIDEO);
  IMG_Init(IMG_INIT_PNG);
  
  COIWindow* window = malloc(sizeof(COIWindow*));
  window->_screen = SDL_CreateWindow("Conquest of Ichabod", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
  window->_renderer = SDL_CreateRenderer(window->_screen, -1, 0);
  window->_currentBoard = NULL;
  return window;
}

void COIWindowDestroy(COIWindow* window) {
  IMG_Quit();
  
  SDL_DestroyRenderer(window->_renderer);
  SDL_DestroyWindow(window->_screen);

  SDL_Quit();
  
  free(window);
}

void COIWindowLoop(COIWindow* window) {
  bool quit = false;
  SDL_Event event;

  while (!quit){
    SDL_Delay(20);
    SDL_PollEvent(&event);

    switch (event.type){
      case SDL_QUIT:
	quit = true;
	break;
    }

    SDL_RenderClear(window->_renderer);
    SDL_RenderPresent(window->_renderer);
  }
}

void COIWindowSetBoard(COIWindow* window, struct COIBoard* board) {
  window->_currentBoard = board;
  SDL_SetRenderDrawColor(window->_renderer,
			 COIBoardBGColor(board, INDEX_RED),
			 COIBoardBGColor(board, INDEX_GREEN),
			 COIBoardBGColor(board, INDEX_BLUE),
			 COIBoardBGColor(board, INDEX_ALPHA));
}

SDL_Renderer* COIWindowGetRenderer(COIWindow* window) {
  return window->_renderer;
}
