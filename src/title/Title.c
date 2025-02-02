#include "Title.h"

static int slideAssetIDs[TITLE_NUM_INTRO_SLIDES] = { 51, 52, 53, 54, 55, 55 };

#ifdef __COI_DEBUG__
static bool shouldSkipIntro = true;
#else
static bool shouldSkipIntro = false;
#endif

void _makeStrings(COIBoard* board, COIString** strings, COITextType* textType) {
  strings[TITLE_STRING_NEW_GAME] = COIStringCreate("New", 290, 150, textType);
  strings[TITLE_STRING_CONTINUE_GAME] = COIStringCreate("Continue", 400, 150, textType);
  strings[TITLE_STRING_QUIT_GAME] = COIStringCreate("Quit", 470, 150, textType);
  strings[TITLE_STRING_OPTIONS] = COIStringCreate("Options", 540, 150, textType);
  for (int i = 0; i < TITLE_NUM_OPTIONS; i++) {
    COIBoardAddString(board, strings[i], 0);
  }
  COIStringPositionRightOfString(strings[TITLE_STRING_CONTINUE_GAME],
				 strings[TITLE_STRING_NEW_GAME],
				 25);
  COIStringPositionRightOfString(strings[TITLE_STRING_QUIT_GAME],
				 strings[TITLE_STRING_CONTINUE_GAME],
				 25);
  COIStringPositionRightOfString(strings[TITLE_STRING_OPTIONS],
        strings[TITLE_STRING_QUIT_GAME],
        25);
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
  COIBoardAddDynamicSprite(board, context->drawing, 0);
  // Slides
  for (int i = 0; i < TITLE_NUM_INTRO_SLIDES; i++) {
    context->slides[i] = COISpriteCreateFromAssetID(0, 0, 640, 480,
						    COI_GLOBAL_LOADER,
						    slideAssetIDs[i],
						    COIWindowGetRenderer(COI_GLOBAL_WINDOW));
    context->slides[i]->_autoHandle = false;
    context->slides[i]->_visible = false;
    COIBoardAddDynamicSprite(board, context->slides[i], 0);
  }

  context->name = COISpriteCreateFromAssetID(280, 0, 360, 136,
					     COI_GLOBAL_LOADER,
					     26,
					     COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  context->name->viewWindowWidth = 90;
  COIBoardAddDynamicSprite(board, context->name, 0);
  COISpriteSetSheetIndex(context->name, 0, 3);
  context->name->_autoHandle = false;
  context->name->_visible = true;
  
  COITextType* white = COITextTypeCreate(12, 255, 255, 255, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COITextType* gray = COITextTypeCreate(12, 120, 120, 120, COIWindowGetRenderer(COI_GLOBAL_WINDOW));

  _makeStrings(board, context->strings, white);
  _makeStrings(board, context->grayStrings, gray);

  _setStringSelected(context, TITLE_STRING_NEW_GAME, true);
  _setStringSelected(context, TITLE_STRING_CONTINUE_GAME, false);
  _setStringSelected(context, TITLE_STRING_QUIT_GAME, false);
  _setStringSelected(context, TITLE_STRING_OPTIONS, false);
  
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

  COIPreferencesMenuInit(&context->prefMenu, board);  
  COIPreferencesMenuSetVisible(&context->prefMenu, false);

  COIBoardSetContext(board, (void*)context);

  COISoundPlay(COI_SOUND_TITLE);

  return board;
}

void titleDestroyBoard(TitleContext* context) {
  COIBoard* board = context->board;
  COIBoardRemoveDynamicSprite(board, context->drawing, 0);
  COIBoardRemoveDynamicSprite(board, context->name, 0);
  COISpriteDestroy(context->drawing);
  COISpriteDestroy(context->name);
  for (int i = 0; i < TITLE_NUM_OPTIONS; i++) {
    COIBoardRemoveString(board, context->strings[i], 0);
    COIBoardRemoveString(board, context->grayStrings[i], 0);
    COIStringDestroy(context->strings[i]);
    COIStringDestroy(context->grayStrings[i]);
  }
  for (int i = 0; i < TITLE_NUM_INTRO_SLIDES; i++) {
    COIBoardRemoveDynamicSprite(board, context->slides[i], 0);
    COISpriteDestroy(context->slides[i]);
  }
  COITextTypeDestroy(context->tBoxTextType);
  TextBoxDestroy(context->textBox);
  KeyboardDestroy(&context->kb, context->board);
  ClassSelectorDestroy(&context->cs, context->board);
  COIPreferencesMenuDeinit(&context->prefMenu, board);
  free(context);
  COIBoardDestroy(board);
  shouldSkipIntro = true; // For next time
}

void _setTextBox(TitleContext* context, TextBox* textBox, char slide) {
  switch (slide) {
  case 0:
    TextBoxSetStrings(textBox,
          "In the year 1032, the Kingdom of Rease triumphed over its enemies in the Ash War.    ",
          "The land obtained from Rease's victory provided the resources necessary for a new renaissance.    ",
          "Rease enjoyed over 200 years of prosperity and innovation.    ",
          "All was well.                    ",
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
        "The King of Rease tasks his most adept mystics with determining the source of power for the horrid spell.    ",
        "They find that Izra's curse is powered by ten corruption cores scattered around Rease.    ",
        "All ten cores must be destroyed before the curse completely corrupts the world.    ",
        "The King sends an open plea to all citizens, requesting their help in this matter.    ",
        NULL);
    break;
  case 4:
  {
    char first[MAX_STRING_SIZE];
    snprintf(first, MAX_STRING_SIZE, "%s, a young %s, has risen to the task.    ", 
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
      "Go forth, young hero. And hurry!    ",
      "Time moves quicker than we think.                       ",
      NULL);
    break;
  default:
    printf("Error when generating intro text.\n");
  }

  TextBoxSetTicksPerChar(textBox, 5);
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
    printf("CURRENT SLIDE: %i\n", context->currentSlide);
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
      } else {
        context->currentSlide++;
      }
      break;
    case TITLE_STRING_QUIT_GAME:
      COISoundPlay(COI_SOUND_SELECT);
      COI_GLOBAL_WINDOW->shouldQuit = true;
      break;
    case TITLE_STRING_OPTIONS:
      COISoundPlay(COI_SOUND_SELECT);
      COIPreferencesMenuSetVisible(&context->prefMenu, true);
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
  // Are we NOT watching intro?
  if (context->currentSlide == -1) {
    int newIndex = context->selectedStringIndex;
    switch (direction) {
    case MOVING_LEFT:
      COISoundPlay(COI_SOUND_BLIP);
      newIndex = MAX(TITLE_STRING_NEW_GAME, context->selectedStringIndex - 1);
      break;
    case MOVING_RIGHT:
      COISoundPlay(COI_SOUND_BLIP);
      newIndex = MIN(TITLE_STRING_OPTIONS, context->selectedStringIndex + 1);
      break;
    case MOVING_SELECT:
      _select(context);
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
}

void titleProcessInput(TitleContext* context, int direction) {
  if (context->prefMenu.frame->_visible) {
    COIPreferencesMenuProcessInput(&context->prefMenu, direction);
    COIBoardQueueDraw(context->board);
  } else if (context->creatingCharacter) {
    _processInputCharacterCreation(context, direction);
    COIBoardQueueDraw(context->board);
  } else {
    _processInputMain(context, direction);
    COIBoardQueueDraw(context->board);
  }
}

