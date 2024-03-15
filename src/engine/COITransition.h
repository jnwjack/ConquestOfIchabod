#ifndef COITRANSITION_H
#define COITRANSITION_H

//#include "COIWindow.h"
#include <stdbool.h>
#include <SDL2/SDL.h>

#define COI_TRANSITION_RECTS_X 20
#define COI_TRANSITION_RECTS_Y 15

struct COITransition;
struct COIWindow;

typedef void (*COITransitionUpdate)(struct COITransition* transition, struct COIWindow* window);

typedef enum COITransitionType {
  COI_TRANSITION_NONE,
  COI_TRANSITION_ENCLOSE,
  COI_TRANSITION_SNAKE
} COITransitionType;

typedef struct COITransition {
  unsigned int currentTick;
  bool complete;
  SDL_Rect rects[COI_TRANSITION_RECTS_X * COI_TRANSITION_RECTS_Y];
  COITransitionUpdate update;
} COITransition;

void COITransitionInit(COITransition* transition, COITransitionType type, struct COIWindow* window);





#endif
