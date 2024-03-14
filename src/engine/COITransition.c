#include "COITransition.h"

void _encloseUpdate(COITransition* transition, COIWindow* window) {
  transition->currentTick++;
  SDL_Renderer* renderer = COIWindowGetRenderer(window);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  
  SDL_RenderFillRect(renderer, &transition->rects[0]);
  SDL_RenderFillRect(renderer, &transition->rects[1]);
  if (transition->currentTick > 50) {
    SDL_RenderFillRect(renderer, &transition->rects[2]);
    SDL_RenderFillRect(renderer, &transition->rects[3]);
  }
  if (transition->currentTick > 100) {
    SDL_RenderFillRect(renderer, &transition->rects[4]);
    SDL_RenderFillRect(renderer, &transition->rects[5]);
  }
  if (transition->currentTick > 150) {
    SDL_RenderFillRect(renderer, &transition->rects[6]);
    SDL_RenderFillRect(renderer, &transition->rects[7]);
  }

  if (transition->currentTick > 200) {
    transition->complete = true;
  }
}

void _encloseInit(COITransition* transition, COIWindow* window) {
  int rectWidth = window->_width / 4;
  for (int i = 0; i < 8; i+=2) {
    transition->rects[i].x = rectWidth * i;
    transition->rects[i].y = 0;
    transition->rects[i].w = rectWidth;
    transition->rects[i].h = window->_height;

    transition->rects[i+1].x = window->_width - 1 - rectWidth * i;
    transition->rects[i+1].y = 0;
    transition->rects[i+1].w = rectWidth;
    transition->rects[i+1].h = window->_height;
  }

  transition->currentTick = 0;
  transition->complete = false;
}

void COITransitionInit(COITransition* transition, COITransitionType type, COIWindow* window) {
  switch (type) {
  case COI_TRANSITION_NONE:
    transition->currentTick = 0;
    transition->complete = true;
    break;
  case COI_TRANSITION_ENCLOSE:
    _encloseInit(transition, window);
    break;
  }
}
