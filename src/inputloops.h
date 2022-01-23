#include "engine/COIBoard.h"
#include "engine/COIWindow.h"
#include "collisions.h"
#include <stdbool.h>

#define MOVING_NONE 0
#define MOVING_LEFT 1
#define MOVING_RIGHT 2
#define MOVING_UP 3
#define MOVING_DOWN 4

void threadTown(COIBoard* board, SDL_Event* event, void* context);
void armory(COIBoard* board, SDL_Event* event, void* context);
