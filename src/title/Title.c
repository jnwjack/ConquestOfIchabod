#include "Title.h"

void _makeStrings(COIBoard* board, COIString** strings, COITextType* textType) {
  strings[TITLE_STRING_NEW_GAME] = COIStringCreate("New", 330, 170, textType);
  strings[TITLE_STRING_CONTINUE_GAME] = COIStringCreate("Continue", 450, 170, textType);
  strings[TITLE_STRING_QUIT_GAME] = COIStringCreate("Quit", 600, 170, textType);
  for (int i = 0; i < TITLE_NUM_OPTIONS; i++) {
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

TitleNextBoard titleGetNextBoard(TitleContext* context) {
  if (context->currentSlide >= TITLE_NUM_INTRO_SLIDES) {
    if (context->selectedStringIndex == TITLE_STRING_NEW_GAME) {
      return TITLE_NEW_GAME;
      // KeyboardInit(&context->kb, context->board);
      // context->drawing->_visible = false;
      return TITLE_TITLE;
    }
    return TITLE_CONTINUE_GAME;
  }

  return TITLE_TITLE;
}

COIBoard* titleCreateBoard() {
  TitleContext* context = malloc(sizeof(TitleContext));
  context->ticks = 0;
  context->animating = false;
  context->currentSlide = -1;
  context->selectedStringIndex = 0;
  COIBoard* board = COIBoardCreate(255, 0, 0, 225, 640, 480, COI_GLOBAL_LOADER);
  context->drawing = COISpriteCreateFromAssetID(0, 0, 640, 480,
						COI_GLOBAL_LOADER,
						25,
						COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  context->drawing->_autoHandle = false;
  context->drawing->_visible = true;
  COIBoardAddDynamicSprite(board, context->drawing);
  // Slides
  for (int i = 0; i < TITLE_NUM_INTRO_SLIDES; i++) {
    context->slides[i] = COISpriteCreateFromAssetID(0, 0, 640, 480,
						    COI_GLOBAL_LOADER,
						    7 + i,
						    COIWindowGetRenderer(COI_GLOBAL_WINDOW));
    context->slides[i]->_autoHandle = false;
    context->slides[i]->_visible = false;
    COIBoardAddDynamicSprite(board, context->slides[i]);
  }

  context->name = COISpriteCreateFromAssetID(280, 0, 360, 136,
					     COI_GLOBAL_LOADER,
					     26,
					     COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  context->name->viewWindowWidth = 90;
  COIBoardAddDynamicSprite(board, context->name);
  COISpriteSetSheetIndex(context->name, 0, 3);
  context->name->_autoHandle = false;
  context->name->_visible = true;
  
  COITextType* white = COITextTypeCreate(25, 255, 255, 255, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COITextType* gray = COITextTypeCreate(25, 120, 120, 120, COIWindowGetRenderer(COI_GLOBAL_WINDOW));

  _makeStrings(board, context->strings, white);
  _makeStrings(board, context->grayStrings, gray);

  _setStringSelected(context, TITLE_STRING_NEW_GAME, true);
  _setStringSelected(context, TITLE_STRING_CONTINUE_GAME, false);
  _setStringSelected(context, TITLE_STRING_QUIT_GAME, false);
  
  COITextTypeDestroy(white);
  COITextTypeDestroy(gray);

  context->tBoxTextType = COITextTypeCreate(25,
					    255, 255, 255,
					    COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  context->textBox = TextBoxCreate(board, context->tBoxTextType);

  context->board = board;

  KeyboardInit(&context->kb, context->board);
  context->drawing->_visible = false;

  COIBoardSetContext(board, (void*)context);
  return board;
}

void titleDestroyBoard(TitleContext* context) {
  COIBoard* board = context->board;
  COIBoardRemoveDynamicSprite(board, context->drawing);
  COIBoardRemoveDynamicSprite(board, context->name);
  COISpriteDestroy(context->drawing);
  COISpriteDestroy(context->name);
  for (int i = 0; i < TITLE_NUM_OPTIONS; i++) {
    COIBoardRemoveString(board, context->strings[i]);
    COIBoardRemoveString(board, context->grayStrings[i]);
    COIStringDestroy(context->strings[i]);
    COIStringDestroy(context->grayStrings[i]);
  }
  for (int i = 0; i < TITLE_NUM_INTRO_SLIDES; i++) {
    COIBoardRemoveDynamicSprite(board, context->slides[i]);
    COISpriteDestroy(context->slides[i]);
  }
  COITextTypeDestroy(context->tBoxTextType);
  TextBoxDestroy(context->textBox);
  free(context);
  COIBoardDestroy(board);
}

void _setTextBox(TextBox* textBox, char slide) {
  switch (slide) {
  case 1:
  case 2:
  case 3:
  case 4:
  case 0:
    TextBoxSetStrings(textBox,
		      "A long time ago, in the same galaxy though.",
		      "You are a hero blah blah blah.",
		      "Something bad happened.",
		      NULL);
    break;
  default:
    printf("Error when generating intro text.\n");
  }
}

void _displaySlide(TitleContext* context) {
  context->ticks++;
  context->name->_visible = false; // jnw: cleanup - only need to do this once
  if (context->ticks > TITLE_TEXT_ANIMATE_TICKS) {
    context->ticks = 0;
    context->drawing->_visible = false;
    if (!context->textBox->box->_visible) {
      // Move to next slide
      if (context->currentSlide > 0) {
	context->slides[context->currentSlide-1]->_visible = false;
      }
      printf("set slide %i to FALSE\n", context->currentSlide-1);
      if (context->currentSlide < TITLE_NUM_INTRO_SLIDES) {
	context->slides[context->currentSlide]->_visible = true;
	printf("set slide %i to TRUE\n", context->currentSlide);
	_setTextBox(context->textBox, context->currentSlide);
      }
      context->currentSlide++;
    } else if (!context->textBox->currentStringDone) {
      TextBoxAnimate(context->textBox);
    } else {
      TextBoxNextString(context->textBox);
    }
    COIBoardQueueDraw(context->board);
  }
}
 

void titleTick(TitleContext* context) {
  if (context->currentSlide > -1 &&
      context->currentSlide < TITLE_NUM_INTRO_SLIDES) {
    _displaySlide(context);
  } else {
    // Animate title
    context->ticks++;
    if (context->ticks >= TITLE_NAME_TICKS) {
      COISpriteSetSheetIndex(context->name, 0, 3);
      COIBoardQueueDraw(context->board);
    }
    if (context->ticks >= TITLE_NAME_TICKS * 5) {
      context->animating = !context->animating;
      if (!context->animating) {
	// Plain static title, no gold.
	COISpriteSetSheetIndex(context->name, 0, 3);
      } else {
	COISpriteSetSheetIndex(context->name, 0, 0);
      }
      COIBoardQueueDraw(context->board);
      context->ticks = 0;
    } else if (context->animating &&
	       context->ticks < TITLE_NAME_TICKS &&
	       context->ticks % (TITLE_NAME_TICKS / 3) == 0) {
      int oldCol = context->name->_srcRect->x / context->name->_srcRect->w;
      COISpriteSetSheetIndex(context->name, 0, (oldCol + 1) % 3);
      COIBoardQueueDraw(context->board);
    }
    
  }
}

void _closeTitle(TitleContext* context) {
  COITransitionInit(&COI_GLOBAL_WINDOW->transition,
                    COI_TRANSITION_ENCLOSE,
                    COI_GLOBAL_WINDOW);
  // Make strings invisible
  for (int i = 0; i < TITLE_NUM_OPTIONS; i++) {
    COIStringSetVisible(context->strings[i], false);
    COIStringSetVisible(context->grayStrings[i], false);
  }
  COIBoardQueueDraw(context->board);
}

void _select(TitleContext* context) {
  if (context->currentSlide == -1) {
    switch (context->selectedStringIndex) {
    case TITLE_STRING_CONTINUE_GAME:
      if (playerSaveExists()) {
        _closeTitle(context);
        // context->currentSlide = TITLE_NUM_INTRO_SLIDES; // Skip intro
        context->currentSlide++;
      }
      break;
    case TITLE_STRING_NEW_GAME:
      _closeTitle(context);
      context->currentSlide++;
      break;
    case TITLE_STRING_QUIT_GAME:
      COI_GLOBAL_WINDOW->shouldQuit = true;
      break;
    default:
      printf("Error in title selection.\n");
    }
  }
}

void titleProcessInput(TitleContext* context, int direction) {
  int newIndex = context->selectedStringIndex;
  switch (direction) {
  case MOVING_LEFT:
    newIndex = MAX(0, context->selectedStringIndex - 1);
    KeyboardMoveCursor(&context->kb, -1, 0);
    COIBoardQueueDraw(context->board);
    break;
  case MOVING_RIGHT:
    newIndex = MIN(2, context->selectedStringIndex + 1);
    KeyboardMoveCursor(&context->kb, 1, 0);
    COIBoardQueueDraw(context->board);
    break;
  case MOVING_UP:
    KeyboardMoveCursor(&context->kb, 0, -1);
    COIBoardQueueDraw(context->board);
    break;
  case MOVING_DOWN:
    KeyboardMoveCursor(&context->kb, 0, 1);
    COIBoardQueueDraw(context->board);
    break;
  case MOVING_SELECT:
    KeyboardAddCharacter(&context->kb, context->board);
    COIBoardQueueDraw(context->board);
    // _select(context);
  default:
    return;
  }

  if (newIndex != context->selectedStringIndex
      && context->currentSlide == -1 /* Are we NOT watching intro? */) {
    _setStringSelected(context, context->selectedStringIndex, false);
    _setStringSelected(context, newIndex, true);
    COIBoardQueueDraw(context->board);
    context->selectedStringIndex = newIndex;
  }
}

