#ifndef COIWINDOW_H
#define COIWINDOW_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#ifdef __WINDOWS__
#define SDL_MAIN_HANDLED
#endif
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "COIBoard.h"
#include "COITransition.h"

#define WINDOW_BASE_WIDTH 640
#define WINDOW_BASE_HEIGHT 480

typedef void (*COILoop)(COIBoard*, SDL_Event* event, void* context);

typedef struct COIWindow {
  SDL_Window* _screen;
  SDL_Renderer* _renderer;
  COIBoard* _currentBoard;
  int _width;
  int _height;
  COILoop _loop;
  void* _loopContext;
  bool shouldQuit;
  COITransition transition;
}COIWindow;

COIWindow* COIWindowCreate();
void COIWindowDestroy(COIWindow* window);

void COIWindowLoop(void* window, bool repeat);
void COIWindowSetBoard(COIWindow* window, COIBoard* board, COILoop loop);
SDL_Renderer* COIWindowGetRenderer(COIWindow* window);
void COIWindowInit();

// Global Window
extern COIWindow* COI_GLOBAL_WINDOW;
#endif
