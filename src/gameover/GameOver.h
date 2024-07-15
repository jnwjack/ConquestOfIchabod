#ifndef GAMEOVER_H
#define GAMEOVER_H

#include "../engine/COI.h"
#include "../player.h"

typedef enum GameOverType {
  GAME_OVER_DEATH,
  GAME_OVER_TIME,
  GAME_OVER_TIME_AND_JOB
} GameOverType;

typedef struct GameOverContext {
  COISprite* background;
  COIBoard* board;
} GameOverContext;

COIBoard* gameOverCreateBoard(COIWindow* window, COIAssetLoader* loader, GameOverType type, PlayerInfo* pInfo);
void gameOverDestroyBoard(GameOverContext* context);


#endif
