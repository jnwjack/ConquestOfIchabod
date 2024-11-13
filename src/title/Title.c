#include "Title.h"

static int slideAssetIDs[TITLE_NUM_INTRO_SLIDES] = { 51, 52, 53, 54, 55, 55 };

static bool shouldSkipIntro = true;

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
  context->creatingCharacter = false;
  COIBoard* board = COIBoardCreate(0, 0, 0, 0, 640, 480, COI_GLOBAL_LOADER);
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
						    slideAssetIDs[i],
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
  
  COITextType* white = COITextTypeCreate(16, 255, 255, 255, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COITextType* gray = COITextTypeCreate(16, 120, 120, 120, COIWindowGetRenderer(COI_GLOBAL_WINDOW));

  _makeStrings(board, context->strings, white);
  _makeStrings(board, context->grayStrings, gray);

  _setStringSelected(context, TITLE_STRING_NEW_GAME, true);
  _setStringSelected(context, TITLE_STRING_CONTINUE_GAME, false);
  _setStringSelected(context, TITLE_STRING_QUIT_GAME, false);
  
  COITextTypeDestroy(white);
  COITextTypeDestroy(gray);

  context->tBoxTextType = COITextTypeCreate(16,
					    255, 255, 255,
					    COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  context->textBox = TextBoxCreate(board, context->tBoxTextType);

  context->board = board;

  KeyboardInit(&context->kb, context->board);
  ClassSelectorInit(&context->cs, context->board);
  KeyboardSetVisible(&context->kb, false);
  ClassSelectorSetVisible(&context->cs, false);

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
  KeyboardDestroy(&context->kb, context->board);
  ClassSelectorDestroy(&context->cs, context->board);
  free(context);
  COIBoardDestroy(board);
}

void _setTextBox(TitleContext* context, TextBox* textBox, char slide) {
  switch (slide) {
  case 0:
    TextBoxSetStrings(textBox,
          "In the year 1032, the Kingdom of Rease triumphed over their enemies in the Ash War.    ",
          "Rease enjoyed over 200 years of peace and prosperity.    ",
          "The land obtained from their victory bolstered their economy.    ",
          "All was well.    ",
          NULL);
    break;
  case 1:
    TextBoxSetStrings(textBox,
        "However, a shadow has descended upon Rease.    ",
        "The evil witch, Izra, brought forth a curse onto the land, corrupting the hearts of its citizens.    ",
        "Our only chance is a young hero...    ",
        NULL);
    break;
  case 2:
    context->textBox->box->_visible = false;
    context->creatingCharacter = true;
    ClassSelectorSetVisible(&context->cs, true);
    break;
  case 3:
    TextBoxSetStrings(textBox,
        "Izra's curse is powered by 10 cores scattered around Rease.    ",
        "All 10 cores must be destroyed before the curse completely corrupts the world.    ",
        NULL);
    break;
  case 4:
  {
    char first[MAX_STRING_SIZE];
    snprintf(first, MAX_STRING_SIZE, "%s, a %s has risen to the challenge.    ", 
            context->kb.name, 
            playerClassNameFromID(context->cs.currentClass));
    char second[MAX_STRING_SIZE];
    snprintf(second, MAX_STRING_SIZE, "%s decides to start in Thread Town, where the first of the cores is rumored to be.    ", 
            context->kb.name);
    TextBoxSetStrings(textBox,
          first,
          second,
          NULL);
    break;
  }
  case 5:
    TextBoxSetStrings(textBox,
      "All will be lost in 300 days. Go forth, young hero!    ",
      "Time moves quicker than we think.                       ",
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
        _setTextBox(context, context->textBox, context->currentSlide);
      }
      if (shouldSkipIntro && context->currentSlide > 2) {
        context->currentSlide = TITLE_NUM_INTRO_SLIDES;
      } else {
        context->currentSlide++;
      }
    } else if (!context->textBox->currentStringDone) {
      TextBoxAnimate(context->textBox);
    } else {
      TextBoxNextString(context->textBox);
    }
    COIBoardQueueDraw(context->board);
  }
}
 

void titleTick(TitleContext* context) {
  if (context->creatingCharacter) {
    // Do nothing
  } else if (context->currentSlide > -1 &&
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
        COISoundPlay(COI_SOUND_SELECT);
        _closeTitle(context);
        context->currentSlide = TITLE_NUM_INTRO_SLIDES - 1; // Skip intro
        // context->currentSlide++;
      } else {
        COISoundPlay(COI_SOUND_INVALID);
      }
      break;
    case TITLE_STRING_NEW_GAME:
      COISoundPlay(COI_SOUND_SELECT);
      _closeTitle(context);
      if (shouldSkipIntro) {
        context->currentSlide = 2;
        // context->currentSlide = TITLE_NUM_INTRO_SLIDES - 1;
        // context->textBox->box->_visible = false;
        // context->creatingCharacter = true;
        // context->slides[context->currentSlide]->_visible = true;
        // ClassSelectorSetVisible(&context->cs, true);
      } else {
        context->currentSlide++;
        shouldSkipIntro = true;
      }
      break;
    case TITLE_STRING_QUIT_GAME:
      COISoundPlay(COI_SOUND_SELECT);
      COI_GLOBAL_WINDOW->shouldQuit = true;
      break;
    default:
      printf("Error in title selection.\n");
    }
  }
}

void _processInputCharacterCreation(TitleContext* context, int direction) {
  switch (direction) {
  case MOVING_LEFT:
    COISoundPlay(COI_SOUND_BLIP);
    if (!KeyboardIsVisible(&context->kb)) {
      ClassSelectorChange(&context->cs, -1);
    } else {
      KeyboardMoveCursor(&context->kb, -1, 0);
    }
    break;
  case MOVING_RIGHT:
    COISoundPlay(COI_SOUND_BLIP);
    if (!KeyboardIsVisible(&context->kb)) {
      ClassSelectorChange(&context->cs, 1);
    } else {
      KeyboardMoveCursor(&context->kb, 1, 0);
    }
    break;
  case MOVING_UP:
    COISoundPlay(COI_SOUND_BLIP);
    KeyboardMoveCursor(&context->kb, 0, -1);
    break;
  case MOVING_DOWN:
    COISoundPlay(COI_SOUND_BLIP);
    KeyboardMoveCursor(&context->kb, 0, 1);
    break;
  case MOVING_SELECT:
    COISoundPlay(COI_SOUND_SELECT);
    if (!KeyboardIsVisible(&context->kb)) {
      COISoundPlay(COI_SOUND_SELECT);
      KeyboardSetVisible(&context->kb, true);
    } else {
      if (KeyboardSelect(&context->kb, context->board)) {
        context->creatingCharacter = false;
        ClassSelectorSetVisible(&context->cs, false);
        KeyboardSetVisible(&context->kb, false);
	      COITransitionInit(&COI_GLOBAL_WINDOW->transition,
                          COI_TRANSITION_ENCLOSE,
                          COI_GLOBAL_WINDOW);
      }
    }
    break;
  case MOVING_DELETE:
    COISoundPlay(COI_SOUND_INVALID);
    if (context->kb.currentNameChar == 0) {
      KeyboardSetVisible(&context->kb, false);
    } else {
      KeyboardRemoveCharacter(&context->kb, context->board);
    }
    break;
  default:
    return;
  }

  COIBoardQueueDraw(context->board);
}

void _processInputMain(TitleContext* context, int direction) {
  int newIndex = context->selectedStringIndex;
  switch (direction) {
  case MOVING_LEFT:
    COISoundPlay(COI_SOUND_BLIP);
    newIndex = MAX(0, context->selectedStringIndex - 1);
    break;
  case MOVING_RIGHT:
    COISoundPlay(COI_SOUND_BLIP);
    newIndex = MIN(2, context->selectedStringIndex + 1);
    break;
  case MOVING_SELECT:
    _select(context);
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

void titleProcessInput(TitleContext* context, int direction) {
  if (context->creatingCharacter) {
    _processInputCharacterCreation(context, direction);
  } else {
    _processInputMain(context, direction);
  }
}

