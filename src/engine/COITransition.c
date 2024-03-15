#include "COITransition.h"
#include "COIWindow.h"

// SNAKE
void _snakeUpdate(COITransition* transition, COIWindow* window) {
  transition->currentTick++;
  SDL_Renderer* renderer = COIWindowGetRenderer(window);

  Uint8 r, g, b, a;
  if (SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a) != 0) {
    printf("Error when retrieving old renderer color values.\n");
  }

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

  int stepSize = 40;
  int ticksPerSquare = 5;
  for (int i = 0; i < transition->currentTick; i++) {
    SDL_RenderFillRect(renderer, &transition->rects[i / ticksPerSquare]);
  }

  // 240 = 8 * 6 * 5
  if (transition->currentTick >= 240) {
    transition->complete = true;
  }

  SDL_SetRenderDrawColor(renderer, r, g, b, a); 
}

void _snakeInit(COITransition* transition, COIWindow* window) {
  int rectsPerRow = 8;
  int rectsPerCol = 6;
  int rectWidth = window->_width / rectsPerRow;
  int rectHeight = window->_height / rectsPerCol;
  int rectIndex = 0;
  for (int y = 0; y < rectsPerCol; y++) {
    for (int x = 0; x < rectsPerRow; x++) {
      if (y % 2 == 0) {
	transition->rects[rectIndex].x = x * rectWidth;
      } else {
	transition->rects[rectIndex].x = ((rectsPerRow - x) - 1) * rectWidth;
      }
      transition->rects[rectIndex].y = y * rectHeight;
      transition->rects[rectIndex].w = rectWidth;
      transition->rects[rectIndex].h = rectHeight;

      rectIndex++;
    }
  }

  transition->currentTick = 0;
  transition->complete = false;
  transition->update = _snakeUpdate;
}

// ENCLOSE
void _encloseUpdate(COITransition* transition, COIWindow* window) {
  transition->currentTick++;
  SDL_Renderer* renderer = COIWindowGetRenderer(window);

  // Save old color values so we can reset it afterwards
  Uint8 r, g, b, a;
  if (SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a) != 0) {
    printf("Error when retrieving old renderer color values.\n");
  }
  
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  
  SDL_RenderFillRect(renderer, &transition->rects[0]);
  SDL_RenderFillRect(renderer, &transition->rects[1]);
  if (transition->currentTick > 25) {
    SDL_RenderFillRect(renderer, &transition->rects[2]);
    SDL_RenderFillRect(renderer, &transition->rects[3]);
  }
  if (transition->currentTick > 50) {
    SDL_RenderFillRect(renderer, &transition->rects[4]);
    SDL_RenderFillRect(renderer, &transition->rects[5]);
  }
  if (transition->currentTick > 75) {
    SDL_RenderFillRect(renderer, &transition->rects[6]);
    SDL_RenderFillRect(renderer, &transition->rects[7]);
  }

  if (transition->currentTick > 150) {
    transition->complete = true;
    // Account for changing time state. Probably a better place to put this
    SDL_SetRenderDrawColor(renderer,
			   COIBoardBGColor(window->_currentBoard, INDEX_RED),
			   COIBoardBGColor(window->_currentBoard, INDEX_GREEN),
			   COIBoardBGColor(window->_currentBoard, INDEX_BLUE),
			   COIBoardBGColor(window->_currentBoard, INDEX_ALPHA));
  } else {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);    
  }


}

void _encloseInit(COITransition* transition, COIWindow* window) {
  int rectWidth = window->_width / 8;
  for (int i = 0; i < 8; i+=2) {
    transition->rects[i].x = rectWidth * (i/2);
    transition->rects[i].y = 0;
    transition->rects[i].w = rectWidth;
    transition->rects[i].h = window->_height;

    transition->rects[i+1].x = window->_width - 1 - rectWidth * ((i/2) + 1);
    transition->rects[i+1].y = 0;
    transition->rects[i+1].w = rectWidth;
    transition->rects[i+1].h = window->_height;
  }

  transition->currentTick = 0;
  transition->complete = false;
  transition->update = _encloseUpdate;
}

void COITransitionInit(COITransition* transition, COITransitionType type, COIWindow* window) {
  switch (type) {
  case COI_TRANSITION_NONE:
    transition->currentTick = 0;
    transition->complete = true;
    transition->update = NULL;
    break;
  case COI_TRANSITION_ENCLOSE:
    _encloseInit(transition, window);
    break;
  case COI_TRANSITION_SNAKE:
    _snakeInit(transition, window);
    break;
  }
}
