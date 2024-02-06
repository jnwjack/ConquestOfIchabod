#include "RentHouse.h"

static void _heal(RentHouseContext* context) {
  Actor* player = context->pInfo->party[0];
  
  char temp[MAX_STRING_SIZE];
  if (!context->pInfo->alreadyHealed) {
    int amountHealed = player->hpMax - player->hp;
    snprintf(temp, MAX_STRING_SIZE, "%i damage healed.", amountHealed);
    TextBoxSetStrings(context->textBox,
		      "You tend to your wounds.",
		      temp,
		      NULL);
    context->pInfo->alreadyHealed = true;
    player->hp = player->hpMax;
  } else {
    TextBoxSetStrings(context->textBox,
		      "You must sleep before you may heal again.",
		      NULL);
  }
}

COIBoard* RentHouseCreateBoard(PlayerInfo* pInfo, COIBoard* outsideBoard) {
  RentHouseContext* context = malloc(sizeof(RentHouseContext));
  context->board = COIBoardCreate(107, 112, 96, 255, 640, 480, COI_GLOBAL_LOADER);
  context->textType = COITextTypeCreate(25, 255, 255, 255, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  context->outsideBoard = outsideBoard;
  context->pInfo = pInfo;
  context->textBox = TextBoxCreate(context->board, context->textType);

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
  COIMenuSetVisible(context->baseMenu);
  
  COIBoardSetContext(context->board, (void*)context);

  return context->board;
}

void RentHouseProcessDirectionalInput(RentHouseContext* context, int event) {
  if (!context->textBox->box->_visible) {
    COIMenuHandleInput(context->baseMenu, event);
    COIBoardQueueDraw(context->board);
  }
}

// Return true when we should exit
bool RentHouseProcessSelectionInput(RentHouseContext* context) {
  if (context->textBox->box->_visible) {
    TextBoxNextString(context->textBox);
    COIBoardQueueDraw(context->board);
  } else {
    switch (COIMenuGetCurrentValue(context->baseMenu)) {
    case RENT_HOUSE_HEAL:
      _heal(context);
      break;
    case RENT_HOUSE_EXIT:
      return true;
    default:
      printf("invalid selection\n");
    }
  }
  
  return false;
}

void RentHouseTick(RentHouseContext* context) {
  if (context->textBox->box->_visible &&
      !context->textBox->currentStringDone) {
    TextBoxAnimate(context->textBox);
    COIBoardQueueDraw(context->board);
  }
}

void RentHouseDestroyBoard(RentHouseContext* context) {
  COIMenuDestroyAndFreeComponents(context->baseMenu, context->board);
  COITextTypeDestroy(context->textType);
  TextBoxDestroy(context->textBox);
  COIBoardDestroy(context->board);
  free(context);
}

