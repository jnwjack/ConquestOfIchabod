#include "Title.h"

void _makeStrings(COIBoard* board, COIString** strings, COITextType* textType) {
  strings[TITLE_STRING_NEW_GAME] = COIStringCreate("New", 330, 170, textType);
  strings[TITLE_STRING_CONTINUE_GAME] = COIStringCreate("Continue", 450, 170, textType);
  strings[TITLE_STRING_QUIT_GAME] = COIStringCreate("Quit", 600, 170, textType);
  for (int i = 0; i < 3; i++) {
    COIBoardAddString(board, strings[i]);
  }
  COIStringPositionRightOfString(strings[TITLE_STRING_CONTINUE_GAME],
				 strings[TITLE_STRING_NEW_GAME],
				 30);
  COIStringPositionRightOfString(strings[TITLE_STRING_QUIT_GAME],
				 strings[TITLE_STRING_CONTINUE_GAME],
				 30);
}

void _setStringSelected(TitleContext* context, int index, bool selected) {
  COIStringSetVisible(context->strings[index], selected);
  COIStringSetVisible(context->grayStrings[index], !selected);
}

COIBoard* titleCreateBoard() {
  TitleContext* context = malloc(sizeof(TitleContext));
  context->nameTicks = 0;
  context->animating = false;
  context->selectedStringIndex = 0;
  COIBoard* board = COIBoardCreate(0, 0, 0, 225, 640, 480, COI_GLOBAL_LOADER);
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
  context->name->_autoHandle = false;

  COITextType* white = COITextTypeCreate(25, 255, 255, 255, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COITextType* gray = COITextTypeCreate(25, 120, 120, 120, COIWindowGetRenderer(COI_GLOBAL_WINDOW));

  _makeStrings(board, context->strings, white);
  _makeStrings(board, context->grayStrings, gray);

  _setStringSelected(context, TITLE_STRING_NEW_GAME, true);
  _setStringSelected(context, TITLE_STRING_CONTINUE_GAME, false);
  _setStringSelected(context, TITLE_STRING_QUIT_GAME, false);
  
  COITextTypeDestroy(white);
  COITextTypeDestroy(gray);

  context->board = board;
  COIBoardSetContext(board, (void*)context);
  return board;
}

void titleDestroyBoard(TitleContext* context) {
  COIBoard* board = context->board;
  COIBoardRemoveDynamicSprite(board, context->drawing);
  COIBoardRemoveDynamicSprite(board, context->name);
  for (int i = 0; i < 3; i++) {
    COIBoardRemoveString(board, context->strings[i]);
    COIBoardRemoveString(board, context->grayStrings[i]);
    COIStringDestroy(context->strings[i]);
    COIStringDestroy(context->grayStrings[i]);
  }
  free(context);
  
  COIBoardDestroy(board);
}

void titleTick(TitleContext* context) {
  context->nameTicks++;
  if (context->animating && context->nameTicks % (TITLE_NAME_TICKS / 3) == 0) {
    int oldCol = context->name->_srcRect->x / context->name->_srcRect->w;
    COISpriteSetSheetIndex(context->name, 0, (oldCol + 1) % 3);
    COIBoardQueueDraw(context->board);
  }
  if (context->nameTicks > TITLE_NAME_TICKS) {
    context->animating = !context->animating;
    context->name->_visible = context->animating;
    COIBoardQueueDraw(context->board);
    context->nameTicks = 0;
  }
}

void titleProcessInput(TitleContext* context, int direction) {
  int newIndex;
  switch (direction) {
  case MOVING_LEFT:
    newIndex = MAX(0, context->selectedStringIndex - 1);
    break;
  case MOVING_RIGHT:
    newIndex = MIN(2, context->selectedStringIndex + 1);
    break;
  default:
    return;
  }

  if (newIndex != context->selectedStringIndex) {
    _setStringSelected(context, context->selectedStringIndex, false);
    _setStringSelected(context, newIndex, true);
    COIBoardQueueDraw(context->board);
    context->selectedStringIndex = newIndex;
  }
}

