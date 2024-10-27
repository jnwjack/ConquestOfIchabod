#include "COIWindow.h"
#include "COISound.h"

COIWindow* COI_GLOBAL_WINDOW = NULL;

COIWindow* COIWindowCreate() {
  // Initialize SDL
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  COISoundInit();
  IMG_Init(IMG_INIT_PNG);
  TTF_Init();

  
  COIWindow* window = malloc(sizeof(COIWindow));
  window->_width = 640;
  window->_height = 480;
  window->_screen = SDL_CreateWindow("Conquest of Ichabod", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window->_width, window->_height, 0);
  window->_renderer = SDL_CreateRenderer(window->_screen, -1, 0);
  window->_currentBoard = NULL;
  window->_loop = NULL;
  window->shouldQuit = false;
  COITransitionInit(&window->transition, COI_TRANSITION_NONE, window);
  return window;
}

void COIWindowDestroy(COIWindow* window) {
  IMG_Quit();
  SDL_DestroyRenderer(window->_renderer);
  SDL_DestroyWindow(window->_screen);

  SDL_Quit();
  
  TTF_Quit();

  free(window);
}

void COIWindowLoop(void* window_v, bool repeat) {
  COIWindow* window = (COIWindow*)window_v;
  SDL_Event event;
  bool shouldDraw = true;

  while (!window->shouldQuit){
    SDL_Delay(20);
    SDL_PollEvent(&event);

    // Enable user control if no active transition
    if (window->transition.complete) {
      switch (event.type){
      case SDL_QUIT:
	      window->shouldQuit = true;
	      break;
      default:
	if (window->_loop) {
	  window->_loop(window->_currentBoard, &event, window->_currentBoard->context);
	}
	break;
      }
    }
    
    if (window->_currentBoard->_shouldDraw) {
      printf("should draw\n");
      SDL_RenderClear(window->_renderer);

      COIBoardUpdateSpriteVisibility(window->_currentBoard);
      COISprite* sprite;

      int extentY = MIN((window->_currentBoard->_frameY + window->_currentBoard->_frameHeight) / COIBOARD_GRID_SIZE + 1, 
                        window->_currentBoard->spriteGridHeight);
      int extentX = MIN((window->_currentBoard->_frameX + window->_currentBoard->_frameWidth) / COIBOARD_GRID_SIZE + 1,
                        window->_currentBoard->spriteGridWidth);
      for (int y = window->_currentBoard->_frameY / COIBOARD_GRID_SIZE; y < extentY; y++) {
        for (int x = window->_currentBoard->_frameX / COIBOARD_GRID_SIZE; x < extentX; x++) {
      // for (int y = 0; y < window->_currentBoard->spriteGridHeight; y++) {
      //   for (int x = 0; x < window->_currentBoard->spriteGridWidth; x++) {
          int index = y * window->_currentBoard->spriteGridWidth + x;
          if (window->_currentBoard->spriteGrid[index]) {
            sprite = window->_currentBoard->spriteGrid[index];
            COIBoardAdjustSprite(window->_currentBoard, sprite);
            if (sprite->_autoHandle) {
              SDL_RenderCopy(window->_renderer, sprite->_texture, sprite->_srcRect, sprite->_drawRect);
            }
          }
        }
      }

      // Draw persistent sprites
      for (int i = 0; i < window->_currentBoard->perSpriteCount; i++) {
	      sprite = window->_currentBoard->persistentSprites[i];
        if (sprite->_visible) {
          SDL_RenderCopy(window->_renderer, sprite->_texture, sprite->_srcRect, sprite->_drawRect);
        }
      }

      // Draw dynamic sprites
      LinkedListResetCursor(window->_currentBoard->dynamicSprites);
      sprite = (COISprite*)LinkedListNext(window->_currentBoard->dynamicSprites);
      while (sprite != NULL) {
        if (sprite->_visible) {
          SDL_RenderCopy(window->_renderer, sprite->_texture, sprite->_srcRect, sprite->_drawRect);
        }
	      sprite = (COISprite*)LinkedListNext(window->_currentBoard->dynamicSprites);
      }

      // New method of handling strings
      COIString** strings = window->_currentBoard->strings;
      for (int i = 0; i < window->_currentBoard->stringCount; i++) {
        if (strings[i]->visible) {
          COIStringDraw(strings[i], window->_renderer);
        }
      }

      if (!window->transition.complete) {
	      window->transition.update(&window->transition, window);
      } else {
        // We'll want to keep drawing as long as transition is active
        window->_currentBoard->_shouldDraw = false;
      }
      
      SDL_RenderPresent(window->_renderer);
      
    }

    if (!repeat) {
      return;
    }
  }
}

void COIWindowSetBoard(COIWindow* window, COIBoard* board, COILoop loop) {
  board->_frameWidth = window->_width;
  board->_frameHeight = window->_height;
  window->_currentBoard = board;
  window->_loop = loop;
  COIBoardQueueDraw(window->_currentBoard);
  SDL_SetRenderDrawColor(window->_renderer,
			 COIBoardBGColor(board, INDEX_RED),
			 COIBoardBGColor(board, INDEX_GREEN),
			 COIBoardBGColor(board, INDEX_BLUE),
			 COIBoardBGColor(board, INDEX_ALPHA));
}

SDL_Renderer* COIWindowGetRenderer(COIWindow* window) {
  return window->_renderer;
}

void COIWindowInit() {
  COI_GLOBAL_WINDOW = COIWindowCreate();
}



