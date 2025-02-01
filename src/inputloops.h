#ifndef INPUTLOOPS_H
#define INPUTLOOPS_H

/*#include "engine/COIMenu.h"
#include "engine/COIBoard.h"
#include "engine/COIWindow.h"
#include "engine/COIExtraCollision.h"*/
#include "engine/COI.h"
#include "armory/Armory.h"
#include "battle/Battle.h"
#include "gameover/GameOver.h"
#include "threadtown/Town.h"
#include "renthouse/RentHouse.h"
#include "title/Title.h"
#include "TimeState.h"
#include <stdbool.h>

void threadTownCallback(COIBoard* board, void* context);
void titleCallback(COIBoard* board, void* context);
void armoryCallback(COIBoard* board, void* context);
void battleCallback(COIBoard* board, void* context);
void rentHouseCallback(COIBoard* board, void* context);
void gameOverCallback(COIBoard* board, void* context);

void threadTown(COIBoard* board, SDL_Event* event, void* context);
void title(COIBoard* board, SDL_Event* event, void* context);
void armory(COIBoard* board, SDL_Event* event, void* context);
void battle(COIBoard* board, SDL_Event* event, void* context);
void rentHouse(COIBoard* board, SDL_Event* event, void* context);
void gameOver(COIBoard* board, SDL_Event* event, void* context);

#endif
