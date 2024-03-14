#ifndef COITRANSITION_H
#define COITRANSITION_H

#include "COIWindow.h"
#include <stdbool.h>

#define COI_TRANSITION_RECTS_X 20
#define COI_TRANSITION_RECTS_Y 15

typedef enum COITransitionType {
  COI_TRANSITION_NONE,
  COI_TRANSITION_ENCLOSE
} COITransitionType;

typedef struct COITransition {
  unsigned int currentTick;
  bool complete;
  SDL_Rect rects[COI_TRANSITION_RECTS_X * COI_TRANSITION_RECTS_Y];
} COITransition;

typedef void (*COITransitionUpdate)(struct COITransition* transition, COIWindow* window);

#endif
