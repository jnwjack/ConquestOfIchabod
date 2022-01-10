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
    //SDL_Delay(1);
    SDL_PollEvent(&event);

    switch (event.type){
      case SDL_QUIT:
	quit = true;
	break;
      case SDL_KEYDOWN:
	switch (event.key.keysym.sym) {
	  case SDLK_LEFT:
	    COIBoardShiftFrameX(window->_currentBoard, -1, window->_width);
	    shouldDraw = true;
	    break;
	  case SDLK_RIGHT:
	    COIBoardShiftFrameX(window->_currentBoard, 1, window->_width);
	    shouldDraw = true;
	    break;
	  case SDLK_UP:
	    COIBoardShiftFrameY(window->_currentBoard, -1, window->_height);
	    shouldDraw = true;
	    break;
	  case SDLK_DOWN:
	    COIBoardShiftFrameY(window->_currentBoard, 1, window->_width);
	    shouldDraw = true;
	    break;
	}
	break;
    }

    if (shouldDraw) {
      SDL_RenderClear(window->_renderer);

      COIBoardUpdateSpriteVisibility(window->_currentBoard, window->_width, window->_height);
      int i;
      COISprite** sprites = COIBoardGetSprites(window->_currentBoard);
      COISprite* sprite;
      for (i = 0; i < COIBoardGetSpriteCount(window->_currentBoard); i++) {
	sprite = sprites[i];
	if (sprite->_visible) {
	  SDL_RenderCopy(window->_renderer, sprite->_texture, NULL, sprite->_rect);
	}
      }
      
      SDL_RenderPresent(window->_renderer);
      shouldDraw = false;
    }
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
