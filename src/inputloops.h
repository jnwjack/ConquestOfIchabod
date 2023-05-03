#ifndef INPUTLOOPS_H
#define INPUTLOOPS_H

#include "engine/COIMenu.h"
#include "engine/COIBoard.h"
#include "engine/COIWindow.h"
#include "collisions.h"
#include "armory/Armory.h"
#include "battle/Battle.h"
#include "threadtown/Town.h"
#include <stdbool.h>

void threadTown(COIBoard* board, SDL_Event* event, void* context);
void armory(COIBoard* board, SDL_Event* event, void* context);

#endif
