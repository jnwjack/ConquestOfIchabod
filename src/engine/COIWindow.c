#include "COIWindow.h"

COIWindow* COIWindowCreate() {
  // Initialize SDL
  SDL_Init(SDL_INIT_VIDEO);
  IMG_Init(IMG_INIT_PNG);
  
  COIWindow* window = malloc(sizeof(COIWindow));
  window->_width = 640;
  window->_height = 480;
  window->_screen = SDL_CreateWindow("Conquest of Ichabod", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window->_width, window->_height, 0);
  window->_renderer = SDL_CreateRenderer(window->_screen, -1, 0);
  window->_currentBoard = NULL;
  window->_loop = NULL;
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
  bool shouldDraw = true;

  while (!quit){
    SDL_Delay(20);
    SDL_PollEvent(&event);

    switch (event.type){
      case SDL_QUIT:
	quit = true;
	break;
      case SDL_KEYDOWN:
	if (window->_loop) {
	  window->_loop(window->_currentBoard, event.key.keysym.sym);
	}
	break;
    }

    if (window->_currentBoard->_shouldDraw) {
      printf("redrawing...\n");
      SDL_RenderClear(window->_renderer);

      COIBoardUpdateSpriteVisibility(window->_currentBoard);
      int i;
      COISprite** sprites = COIBoardGetSprites(window->_currentBoard);
      COISprite* sprite;
      for (i = 0; i < COIBoardGetSpriteCount(window->_currentBoard); i++) {
	sprite = sprites[i];
	if (sprite->_visible) {
	  SDL_RenderCopy(window->_renderer, sprite->_texture, NULL, sprite->_drawRect);
	}
      }
      
      SDL_RenderPresent(window->_renderer);
      window->_currentBoard->_shouldDraw = false;
    }
  }
}

void COIWindowSetBoard(COIWindow* window, struct COIBoard* board) {
  board->_frameWidth = window->_width;
  board->_frameHeight = window->_height;
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

void COIWindowSetLoop(COIWindow* window, COILoop loop) {
  window->_loop = loop;
}
