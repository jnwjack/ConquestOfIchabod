#include "Town.h"

COIBoard* townCreateBoard(COIWindow* window, COIAssetLoader* loader, PlayerInfo* pInfo) {
  COIBoard* board = COIBoardCreate(2, 132, 28, 255, 5000, 5000, loader);
  COIBoardLoadSpriteMap(board, COIWindowGetRenderer(window), "src/threadtown/spritemap.dat");
  TownContext* context = malloc(sizeof(TownContext));
  context->window = window;
  context->pInfo = pInfo;
  context->direction = MOVING_NONE;
  context->terrain = TT_SAFE;
  context->terrainTicks = 0;
  // Only 1 dynamic sprite: the player
  COISprite** dynSprites = actorGetSpriteList(context->pInfo->party, 1);
  COIBoardSetDynamicSprites(board, dynSprites, 1);

  COIBoardSetContext(board, (void*)context);

  return board;
}

// After a certain amount of ticks, check if we should enter a battle (random encounter).
// The likelihood of entering a battle is based off of the current terrain.
bool townCheckForBattle(TownContext* context) {
  if (context->terrainTicks >= TOWN_BATTLE_TICKS) {
    context->terrainTicks = 0;
    switch (context->terrain) {
    case TT_THICK_GRASS:
      return generateRandomBoolWeighted(0.3);
    default:
      return false;
    }
  } else {
    context->terrainTicks++;
    return false;
  }
}

void townUpdateTerrain(TownContext* context, int collisionResult) {
  switch (collisionResult) {
  case THICK_GRASS:
    context->terrain = TT_THICK_GRASS;
    break;
  case COI_NO_COLLISION:
    context->terrain = TT_SAFE;
    break;
  default:
    break;
    // Do nothing
  }
}


void townDestroyBoard(COIBoard* board) {
  COIBoardDestroy(board);
}
