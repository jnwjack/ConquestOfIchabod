#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "COIBoard.h"

#ifndef COIWINDOW_H
#define COIWINDOW_H

typedef void (*COILoop)(COIBoard*, SDL_Event* event, void* context);

typedef struct COIWindow {
  SDL_Window* _screen;
  SDL_Renderer* _renderer;
  COIBoard* _currentBoard;
  int _width;
  int _height;
  COILoop _loop;
  void* _loopContext;
}COIWindow;

COIWindow* COIWindowCreate();
void COIWindowDestroy(COIWindow* window);

void COIWindowLoop(COIWindow* window);
void COIWindowSetBoard(COIWindow* window, struct COIBoard* board);
SDL_Renderer* COIWindowGetRenderer(COIWindow* window);
void COIWindowSetLoop(COIWindow* window, COILoop loop, void* context);


#endif
