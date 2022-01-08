#ifndef COIWINDOW_H
#define COIWINDOW_H

#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "COIBoard.h"

typedef struct COIWindow {
  SDL_Window* _screen;
  SDL_Renderer* _renderer;
  COIBoard* _currentBoard;
}COIWindow;

COIWindow* COIWindowCreate();
void COIWindowDestroy(COIWindow* window);

void COIWindowLoop(COIWindow* window);
void COIWindowSetBoard(COIWindow* window, COIBoard* board);

#endif
