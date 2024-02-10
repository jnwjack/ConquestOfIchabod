#ifndef GAMEOVER_H
#define GAMEOVER_H

#include "../engine/COI.h"

typedef struct GameOverContext {
  int test;
} GameOverContext;

COIBoard* gameOverCreateBoard(COIWindow* window, COIAssetLoader* loader);


#endif
