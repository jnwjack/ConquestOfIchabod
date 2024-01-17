#include "COIMenu.h"

COIMenu* COIMenuCreate(COISprite* frame, COISprite* pointer) {
  COIMenu* menu = malloc(sizeof(COIMenu));
  menu->_frame = frame;
  menu->_pointer = pointer;
  menu->_x = frame->_x;
  menu->_y = frame->_y;
  menu->_width = frame->_width;
  menu->_height = frame->_height;
  menu->_stringCount = 0;
  menu->_strings = NULL;
  menu->_values = NULL;
  menu->_maxStrings = 1000; // Added for new version of COIMenu
  menu->_current = 0;
  menu->_fontSize = 0;
  menu->_visibleTextCount = 0;
  menu->_lowerFrameBound = 0;
  menu->_upperFrameBound = 0;

  menu->_frame->_autoHandle = false;
  menu->_pointer->_autoHandle = false;

  COIMenuAdjustFrame(menu);

  return menu;
}

// New initializer. Trying to get a version of COIMenu where
// we can add strings one by one instead of all at once.
COIMenu* COIMenuCreateWithCapacity(COISprite* frame, COISprite* pointer, int capacity) {
  COIMenu* menu = COIMenuCreate(frame, pointer);
  menu->_strings = malloc(capacity * sizeof(COIString*));
  menu->_values = malloc(capacity * sizeof(COIString*));
  menu->_maxStrings = capacity;
}

// Like COIMenuDestroy but also destroy sprites and strings in menu.
// Assumes sprites are dynamic sprites.
void COIMenuDestroyAndFreeComponents(COIMenu* menu, COIBoard* board) {
  if (menu == NULL) {
    return;
  }
  COISprite* frame = menu->_frame;
  COIBoardRemoveDynamicSprite(board, frame);
  COISpriteDestroy(frame);
  COISprite* pointer = menu->_pointer;
  COIBoardRemoveDynamicSprite(board, pointer);
  COISpriteDestroy(pointer);
  if (menu->_strings != NULL) {
    for (int i = 0; i < menu->_stringCount; i++) {
      COIString* str = menu->_strings[i];
      COIBoardRemoveString(board, str);
      COIStringDestroy(str);
    }
  }
  COIMenuDestroy(menu);
}

void COIMenuDestroy(COIMenu* menu) {
  if (menu == NULL) {
    return;
  }
  if (menu->_strings != NULL) {
    free(menu->_strings);
  }
  free(menu);
}

// Only sets the texts within the frame bounds to be visible.
// Sets others to be false
void COIMenuSetVisible(COIMenu* menu) {
  menu->_frame->_visible = true;
  menu->_pointer->_visible = true;

  // Corresponds to a slot between the upper and lower frame bounds
  int slot = 0;
  
  int i;
  COIString* string;
  for (i = 0; i < menu->_stringCount; i++) {
    string = menu->_strings[i];

    if (i >= menu->_lowerFrameBound && i <= menu->_upperFrameBound) {
      COIStringSetVisible(string, true);
      COIStringSetPos(string,
		    menu->_x + COI_MENU_OFFSET_X,
		    menu->_y + COI_MENU_OFFSET_Y + (slot * (menu->_fontSize + COI_PADDING)));
      slot++;
    } else {
      COIStringSetVisible(string, false);
    }
  }
}

// Sets all texts false
void COIMenuSetInvisible(COIMenu* menu) {
  menu->_frame->_visible = false;
  menu->_pointer->_visible = false;
  int i;
  COIString* string;
  for (i = 0; i < menu->_stringCount; i++) {
    string = menu->_strings[i];
    COIStringSetVisible(string, false);
  }
}

static void _updateBoundsAndNumVisibleStrings(COIMenu* menu) {
  // Update bounds and visible text count to be based off of string fonts.
  // Using first string in list, assuming all to be same font weight.
  if (menu->_stringCount > 0) {
    menu->_fontSize = menu->_strings[0]->fontSize;
    menu->_visibleTextCount = (menu->_height - COI_MENU_OFFSET_Y) / (menu->_fontSize + COI_PADDING);
    menu->_upperFrameBound = menu->_visibleTextCount - 1;
    // Can't have upper bound greater than lower
    if (menu->_lowerFrameBound > menu->_upperFrameBound) {
      printf("Error when creating menu\n");
      free(menu);
      return;
    }
  } else {
    printf("Setting menu text with zero strings\n");
    return;
  }

  // Font size may have changed. Readjust frame.
  COIMenuAdjustFrame(menu);
}

void COIMenuSetTexts(COIMenu* menu, COIString** strings, int numStrings) {
  /*
  // Update bounds and visible text count to be based off of string fonts.
  // Using first string in list, assuming all to be same font weight.
  if (numStrings > 0) {
    menu->_fontSize = strings[0]->fontSize;
    menu->_visibleTextCount = (menu->_height - COI_MENU_OFFSET_Y) / (menu->_fontSize + COI_PADDING);
    menu->_upperFrameBound = menu->_visibleTextCount - 1;
    // Can't have upper bound greater than lower
    if (menu->_lowerFrameBound > menu->_upperFrameBound) {
      printf("Error when creating menu\n");
      free(menu);
      return;
    }
  } else {
    printf("Setting menu text with zero strings\n");
    return;
  }
  */

  // We don't care about values when using the old way of handling
  // things.
  if (menu->_values != NULL) {
    free(menu->_values);
    menu->_values = NULL;
  }
  
  if (menu->_strings != NULL) {
    free(menu->_strings);
  }
  menu->_strings = malloc(numStrings * sizeof(COIString*));
  menu->_stringCount = numStrings;
  menu->_maxStrings = numStrings;
  for (int i = 0; i < menu->_stringCount; i++) {
    menu->_strings[i] = strings[i];
  }

  _updateBoundsAndNumVisibleStrings(menu);

  // Maybe this isn't needed?
  /*
  if (menu->_stringCount < menu->_visibleTextCount) {
    menu->_visibleTextCount = menu->_stringCount;
    menu->_upperFrameBound = menu->_visibleTextCount - 1;
  }
  */
}

void COIMenuAdjustFrame(COIMenu* menu) {
  if (menu->_current > menu->_upperFrameBound) {
    menu->_upperFrameBound = menu->_current;
    menu->_lowerFrameBound = menu->_current - (menu->_visibleTextCount - 1);
  } else if (menu->_current < menu->_lowerFrameBound)  {
    menu->_lowerFrameBound = menu->_current;
    menu->_upperFrameBound = menu->_current + (menu->_visibleTextCount - 1);
  }

  int pointerLocation = menu->_current - menu->_lowerFrameBound;
  int pointerY = (menu->_y + COI_MENU_OFFSET_Y) + (menu->_fontSize / 2) + (pointerLocation * (menu->_fontSize + COI_PADDING)) - (menu->_pointer->_height / 2);
  int pointerX = (menu->_x + COI_MENU_OFFSET_X) - (menu->_pointer->_width + COI_PADDING);
  COISpriteSetPos(menu->_pointer, pointerX, pointerY);
}

void COIMenuReset(COIMenu* menu) {
  menu->_current = 0;
  COIMenuAdjustFrame(menu);
}

void COIMenuIncrement(COIMenu* menu, int step) {
  int nextCurrent = menu->_current + step;
  if (nextCurrent >= menu->_stringCount) {
    menu->_current = 0;
  } else if (nextCurrent < 0) {
    menu->_current = menu->_stringCount - 1;
  } else {
    menu->_current = nextCurrent;
  }

  COIMenuAdjustFrame(menu);
}

bool COIMenuHandleInput(COIMenu* menu, int event) {
  bool selection = false;
  switch (event) {
    case MOVING_UP:
      COIMenuIncrement(menu, -1);
      COIMenuSetVisible(menu);
      break;
    case MOVING_DOWN:
      COIMenuIncrement(menu, 1);
      COIMenuSetVisible(menu);
      break;
    case MOVING_SELECT:
      selection = true;
      break;
  }

  return selection;
}

// Add string and associated value that's returned when the
// string is selected
bool COIMenuAddString(COIMenu* menu, COIString* string, int val) {
  if (menu->_stringCount >= menu->_maxStrings) {
    return false;
  }

  menu->_strings[menu->_stringCount] = string;
  menu->_values[menu->_stringCount] = val;
  menu->_stringCount++;

  _updateBoundsAndNumVisibleStrings(menu);

  return true;
}

void COIMenuRemoveString(COIMenu* menu, int index, COIBoard* board) {
  COIBoardRemoveString(board, menu->_strings[index]);
  COIStringDestroy(menu->_strings[index]);
  for (int i = index; i + 1 < menu->_stringCount; i++) {
    menu->_strings[i] = menu->_strings[i + 1];
    menu->_values[i] = menu->_values[i + 1];
  }

  _updateBoundsAndNumVisibleStrings(menu);
}

int COIMenuGetCurrentValue(COIMenu* menu) {
  if (menu->_values != NULL) {
    return menu->_values[menu->_current];
  }
  return -1;
}

