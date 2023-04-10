#include "Battle.h"

COIBoard* battleCreateBoard(COIWindow* window, COIAssetLoader* loader, COIBoard* outsideBoard) {
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

  COIBoardSetStrings(board, context->actionStrings, 5);

  COIBoardSetContext(board, (void*)context);

  return board;
}
