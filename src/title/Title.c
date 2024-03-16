#include "Title.h"


COIBoard* titleCreateBoard() {
  TitleContext* context = malloc(sizeof(TitleContext));
  context->nameTicks = 0;
  COIBoard* board = COIBoardCreate(0, 0, 0, 225, 640, 480, COI_GLOBAL_LOADER);
  
  COITextType* textType = COITextTypeCreate(40, 255, 255, 255, COIWindowGetRenderer(COI_GLOBAL_WINDOW));

  context->drawing = COISpriteCreateFromAssetID(0, 0, 640, 480,
						COI_GLOBAL_LOADER,
						25,
						COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COIBoardAddDynamicSprite(board, context->drawing);

  context->name = COISpriteCreateFromAssetID(280, 0, 360, 136,
					     COI_GLOBAL_LOADER,
					     26,
					     COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  context->name->viewWindowWidth = 90;
  COIBoardAddDynamicSprite(board, context->name);
  COISpriteSetSheetIndex(context->name, 0, 0);

  context->board = board;
  COIBoardSetContext(board, (void*)context);
  return board;
}

void titleDestroyBoard(TitleContext* context) {
  COIBoard* board = context->board;
  COIBoardRemoveDynamicSprite(board, context->drawing);
  COIBoardRemoveDynamicSprite(board, context->name);
  free(context);
  
  COIBoardDestroy(board);
}

void titleTick(TitleContext* context) {
  context->nameTicks++;
  if (context->nameTicks > TITLE_NAME_TICKS) {
    int oldCol = context->name->_srcRect->x / context->name->_srcRect->w;
    COISpriteSetSheetIndex(context->name, 0, (oldCol + 1) % 3);
    context->nameTicks = 0;
    COIBoardQueueDraw(context->board);
  }
}

