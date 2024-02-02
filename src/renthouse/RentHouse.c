#include "RentHouse.h"

COIBoard* RentHouseCreateBoard() {
  RentHouseContext* context = malloc(sizeof(RentHouseContext));
  context->board = COIBoardCreate(107, 112, 96, 255, 640, 480, COI_GLOBAL_LOADER);
  context->textType = COITextTypeCreate(25, 255, 255, 255, COIWindowGetRenderer(COI_GLOBAL_WINDOW));


  // Create base menu: Heal, Sleep, Pay Rent, Exit
  COISprite* frame = COISpriteCreateFromAssetID(60, 40, 200, 190,
						COI_GLOBAL_LOADER,
						5,
						COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  frame->_autoHandle = false;
  COIBoardAddDynamicSprite(context->board, frame);
  COISprite* pointer = COISpriteCreateFromAssetID(0, 0, 32, 32,
						  COI_GLOBAL_LOADER,
						  6,
						  COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  pointer->_autoHandle = false;
  COIBoardAddDynamicSprite(context->board, pointer);

  context->baseMenu = COIMenuCreateWithCapacity(frame, pointer, 4);
  COIString* healString = COIStringCreate("Heal", 0, 0, context->textType);
  COIBoardAddString(context->board, healString);
  COIMenuAddString(context->baseMenu, healString, 0);
  
  COIString* sleepString = COIStringCreate("Sleep", 0, 0, context->textType);
  COIBoardAddString(context->board, sleepString);
  COIMenuAddString(context->baseMenu, sleepString, 1);

  COIString* payString = COIStringCreate("Pay Rent", 0, 0, context->textType);
  COIBoardAddString(context->board, payString);
  COIMenuAddString(context->baseMenu, payString, 2);

  COIString* exitString = COIStringCreate("Exit", 0, 0, context->textType);
  COIBoardAddString(context->board, exitString);
  COIMenuAddString(context->baseMenu, exitString, 3);
  
  COIBoardSetContext(context->board, (void*)context);

  return context->board;
}
