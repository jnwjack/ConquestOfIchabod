#include "Battle.h"
#include "../actor.h"

COIBoard* battleCreateBoard(COIWindow* window, COIAssetLoader* loader, COIBoard* outsideBoard, int actorType) {
  COIBoard* board = COIBoardCreate(99, 91, 95, 225, 640, 480, loader);
  COIBoardLoadSpriteMap(board, COIWindowGetRenderer(window), "src/battle/spritemap.dat");

  // Context
  BattleContext* context = malloc(sizeof(BattleContext));
  context->textType = COITextTypeCreate(25, 255, 255, 255, COIWindowGetRenderer(window));
  context->actionStrings[0] = COIStringCreate("Attack", 0, 0, context->textType);
  context->actionStrings[1] = COIStringCreate("Tech", 0, 0, context->textType);
  context->actionStrings[2] = COIStringCreate("Special", 0, 0, context->textType);
  context->actionStrings[3] = COIStringCreate("Item", 0, 0, context->textType);
  context->actionStrings[4] = COIStringCreate("Flee", 0, 0, context->textType);

  COISprite** sprites = COIBoardGetSprites(board);

  context->actionMenu = COIMenuCreate(sprites[3], sprites[4]);
  COIMenuSetTexts(context->actionMenu, context->actionStrings, 5);
  COIMenuIncrement(context->actionMenu, 1);
  COIMenuSetVisible(context->actionMenu);

  // Enemies, can later randomize number
  context->numEnemies = 3;
  int offsetX = 50, offsetY = 100;
  context->enemies = malloc(sizeof(Actor*) * context->numEnemies);
  for (int i = 0; i < context->numEnemies; i++) {
    context->enemies[i] = actorCreateOfType(actorType, offsetX, offsetY + 80*i, loader, window);
  }

  COIBoardSetDynamicSprites(board, actorGetSpriteList(context->enemies, context->numEnemies), context->numEnemies);

  COIBoardSetStrings(board, context->actionStrings, 5);

  COIBoardSetContext(board, (void*)context);

  return board;
}

void battleDestroyBoard(COIBoard* board) {
  BattleContext* context = (BattleContext*)board->context;
  COIMenuDestroy(context->actionMenu);
  COITextTypeDestroy(context->textType);
  for (int i = 0; i < context->numEnemies; i++) {
    free(context->enemies[i]);
  }
  free(context->enemies);
  free(context);
  COIBoardDestroy(board);
}
