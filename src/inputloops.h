#ifndef INPUTLOOPS_H
#define INPUTLOOPS_H

#include "engine/COIMenu.h"
#include "engine/COIBoard.h"
#include "engine/COIWindow.h"
#include "engine/COIExtraCollision.h"
#include "armory/Armory.h"
#include "battle/Battle.h"
#include "gameover/GameOver.h"
#include "threadtown/Town.h"
#include <stdbool.h>

#define OVERWORLD_MOVE_SPEED 5

void threadTown(COIBoard* board, SDL_Event* event, void* context);
void armory(COIBoard* board, SDL_Event* event, void* context);
void battle(COIBoard* board, SDL_Event* event, void* context);

#endif
