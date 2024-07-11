#include "RentHouse.h"

static void _destroyString(COIBoard* board, COIString* string) {
  if (string != NULL) {
    COIBoardRemoveString(board, string);
    COIStringDestroy(string);
  }
}

static void _heal(RentHouseContext* context) {
  Actor* player = context->pInfo->party[0];
  
  
  if (!context->pInfo->alreadyHealed) {
    COISoundPlay(COI_SOUND_SELECT);
    char hpString[MAX_STRING_SIZE];
    char spString[MAX_STRING_SIZE];
    char tpString[MAX_STRING_SIZE];
    int amountHealed = player->hpMax - player->hp;
    int regainedTP = player->tpMax - player->tp;
    int regainedSP = player->spMax - player->sp;
    snprintf(hpString, MAX_STRING_SIZE, "%i damage healed.", amountHealed);
    snprintf(tpString, MAX_STRING_SIZE, "%i TP restored.", regainedTP);
    snprintf(spString, MAX_STRING_SIZE, "%i SP restored.", regainedSP);
    if (!context->pInfo->rentHouseBaldUsed && GLOBAL_TIME.day > 1) {
      TextBoxSetStrings(context->textBox,
		      "You tend to your wounds.",
		      hpString,
		      tpString,
          "Feeling your hair, you notice that you have a bald spot.",
		      spString,
		      NULL);
      context->pInfo->rentHouseBaldUsed = true;
    } else {
      TextBoxSetStrings(context->textBox,
            "You tend to your wounds.",
            hpString,
            tpString,
            spString,
            NULL);
      context->pInfo->alreadyHealed = true;
      player->hp = player->hpMax;
      player->tp = player->tpMax;
      player->sp = player->spMax;
    }
  } else {
    COISoundPlay(COI_SOUND_INVALID);
    TextBoxSetStrings(context->textBox,
		      "You must sleep before you may heal again.",
		      NULL);
  }
}

// _sleep appears to be a reserved function with MINGW
#ifdef __WINDOWS__
static void _sleep_coi(RentHouseContext* context) {
#else
static void _sleep(RentHouseContext* context) {
#endif
// JNW: left off here, can't sleep if evicted
  TextBoxSetStrings(context->textBox,
		    "You sleep...",
		    NULL);
  TimeStateIncrement(12);
  playerCheckForEviction(context->pInfo);
  context->statusWindow._refreshFlags |= RENT_HOUSE_REFRESH_DAYS;
  context->pInfo->alreadyHealed = false;
}

bool _tooEarlyForPayment(PlayerInfo* pInfo) {
  return pInfo->nextRentDate - GLOBAL_TIME.day >= 30;
}

static void _pay(RentHouseContext* context) {
  char temp[MAX_STRING_SIZE];
  if (_tooEarlyForPayment(context->pInfo)) {
    TextBoxSetStrings(context->textBox,
		      "It is too early to pay.",
		      NULL);
  } else if (context->pInfo->inventory->money >= RENT_HOUSE_PRICE &&
      !_tooEarlyForPayment(context->pInfo)) {
    snprintf(temp, MAX_STRING_SIZE, "Rent paid (%i gold).", RENT_HOUSE_PRICE);
    TextBoxSetStrings(context->textBox,
		      temp,
		      NULL);
    context->pInfo->inventory->money -= RENT_HOUSE_PRICE;
    context->pInfo->nextRentDate += 30;
    context->statusWindow._refreshFlags |= (RENT_HOUSE_REFRESH_MONEY |
					     RENT_HOUSE_REFRESH_DAYS);

  } else {
    snprintf(temp, MAX_STRING_SIZE, "You are not able to afford rent (%i gold).", RENT_HOUSE_PRICE);
    TextBoxSetStrings(context->textBox,
		      temp,
		      NULL);
  }
}

// JNW: Can pull this similar structures (_make[BLANK]Window, etc.) into a structure
static void _makeStatWindowStrings(RentHouseContext* context) {
  RHStatusWindow* window = &context->statusWindow;
  char temp[MAX_STRING_SIZE];
  if (window->_refreshFlags & RENT_HOUSE_REFRESH_MONEY) {
    _destroyString(context->board, window->currentGold);
    snprintf(temp, MAX_STRING_SIZE, "Gold: %i", context->pInfo->inventory->money);
    window->currentGold = COIStringCreate(temp, 0, 0, context->textType);
    COIStringConfineToSprite(window->currentGold, window->frame);
    COIStringSetVisible(window->currentGold, true);
    COIBoardAddString(context->board, window->currentGold);
  }
  if ((window->_refreshFlags & RENT_HOUSE_REFRESH_PRICE) > 0) {
    _destroyString(context->board, window->price);
    snprintf(temp, MAX_STRING_SIZE, "Rent: %i", RENT_HOUSE_PRICE);
    window->price = COIStringCreate(temp, 0, 0, context->textType);
    COIStringPositionBelowString(window->price, window->currentGold, false);
    COIStringSetVisible(window->price, true);
    COIBoardAddString(context->board, window->price);
  }
  if (window->_refreshFlags & RENT_HOUSE_REFRESH_DAYS) {
    _destroyString(context->board, window->daysLeft);
    long daysLeft = (long)context->pInfo->nextRentDate - (long)GLOBAL_TIME.day;
    if (daysLeft < 0) {
      window->daysLeft = COIStringCreate("Days Left: LATE", 0, 0, context->textType);
    } else {
      snprintf(temp, MAX_STRING_SIZE, "Days Left: %lu", context->pInfo->nextRentDate - GLOBAL_TIME.day);
      window->daysLeft = COIStringCreate(temp, 0, 0, context->textType);
    }
    COIStringPositionBelowString(window->daysLeft, window->price, false);
    COIStringSetVisible(window->daysLeft, true);
    COIBoardAddString(context->board, window->daysLeft);
  }

  window->_refreshFlags = 0;
}

static void _makeStatWindow(RentHouseContext* context) {
  RHStatusWindow* window = &context->statusWindow;
  
  window->frame = COISpriteCreateFromAssetID(320, 40, 250, 250,
					     COI_GLOBAL_LOADER,
					     5,
					     COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  window->frame->_autoHandle = false;
  window->frame->_visible = true;
  // Update everything on first time
  window->_refreshFlags = (RENT_HOUSE_REFRESH_MONEY |
			   RENT_HOUSE_REFRESH_DAYS |
			   RENT_HOUSE_REFRESH_PRICE);
  window->currentGold = NULL;
  window->daysLeft = NULL;
  window->price = NULL;
  _makeStatWindowStrings(context);
  COIBoardAddDynamicSprite(context->board, window->frame);
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

  _makeStatWindow(context);
  
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
    case RENT_HOUSE_SLEEP:
    #ifdef __WINDOWS__
      _sleep_coi(context);
    #else
      _sleep(context);
    #endif
      break;
    case RENT_HOUSE_PAY:
      _pay(context);
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
  if (!context->textBox->box->_visible) {
    _makeStatWindowStrings(context);
  } else if (!context->textBox->currentStringDone) {
    TextBoxAnimate(context->textBox);
    COIBoardQueueDraw(context->board);
  }
}

void RentHouseDestroyBoard(RentHouseContext* context) {
  COIMenuDestroyAndFreeComponents(context->baseMenu, context->board);
  COITextTypeDestroy(context->textType);
  TextBoxDestroy(context->textBox);
  
  _destroyString(context->board, context->statusWindow.price);
  _destroyString(context->board, context->statusWindow.daysLeft);
  _destroyString(context->board, context->statusWindow.currentGold);
  COIBoardRemoveDynamicSprite(context->board, context->statusWindow.frame);
  COISpriteDestroy(context->statusWindow.frame);

  COIBoardDestroy(context->board);
  free(context);
}

