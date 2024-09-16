#include "GameOver.h"

COIBoard* gameOverCreateBoard(COIWindow* window, COIAssetLoader* loader, GameOverType type, PlayerInfo* pInfo) {
  COIBoard* board = COIBoardCreate(20, 20, 20, 225, 640, 480, loader);
  
  COITextType* textType = COITextTypeCreate(30, 255, 255, 255, COIWindowGetRenderer(window));
  COITextType* smallerTextType = COITextTypeCreate(16, 255, 255, 255, COIWindowGetRenderer(window));

  // Replace with 640 x 480 COISprite that displays "game over" image.
  GameOverContext* context = malloc(sizeof(GameOverContext));
  context->board = board;
  switch (type) {
  case GAME_OVER_DEATH:
  {
    COITextType* deathType = COITextTypeCreate(24, 0, 0, 0, COIWindowGetRenderer(window));
    COIString* nameString = COIStringCreate(pInfo->name, 250, 360, deathType);
    COIBoardAddString(board, nameString);

    context->background = COISpriteCreateFromAssetID(0, 0, 640, 480, loader, 56, COIWindowGetRenderer(window));
    break;
  }
  case GAME_OVER_TIME:
  {
    context->background = COISpriteCreateFromAssetID(0, 0, 640, 480, loader, 57, COIWindowGetRenderer(window));
    COIString* gameOverString = COIStringCreate("Game Over", 220, 200, textType);
    COIBoardAddString(board, gameOverString);
    COIString* explanationString = COIStringCreate("You have run out of time.", 0, 0, smallerTextType);
    COIStringSetPosWithWrapping(explanationString, 220, 264, 300);
    COIBoardAddString(board, explanationString);
    
    break;
  }
  case GAME_OVER_TIME_AND_JOB:
  {
    COITextType* timeAndJobType = COITextTypeCreate(6, 0, 0, 0, COIWindowGetRenderer(window));
    COIString* nameString = COIStringCreate(pInfo->name, 395, 320, timeAndJobType);
    COIBoardAddString(board, nameString);
    COIString* gameOverString = COIStringCreate("Game Over", 10, 200, textType);
    COIBoardAddString(board, gameOverString);
    COIString* explanationString = COIStringCreate("You have run out of time.", 0, 0, smallerTextType);
    COIStringSetPosWithWrapping(explanationString, 10, 264, 300);
    COIBoardAddString(board, explanationString);


    context->background = COISpriteCreateFromAssetID(0, 0, 640, 480, loader, 58, COIWindowGetRenderer(window));
    break;
  }
  }
  COIBoardAddDynamicSprite(board, context->background);

  COIBoardSetContext(board, (void*)context);

  COITextTypeDestroy(textType);
  COITextTypeDestroy(smallerTextType);

  return board;
}

void gameOverDestroyBoard(GameOverContext* context) {
  COIBoardRemoveDynamicSprite(context->board, context->background);
  for (int i = 0; i < context->board->stringCount; i++) {
    COIStringDestroy(context->board->strings[i]);
  }
  COIBoardDestroy(context->board);
}
