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

void COIMenuSetTexts(COIMenu* menu, COIString** strings, int numStrings) {
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
  
  if (menu->_strings != NULL) {
    free(menu->_strings);
  }
  menu->_strings = malloc(numStrings * sizeof(COIString*));
  menu->_stringCount = numStrings;
  for (int i = 0; i < menu->_stringCount; i++) {
    menu->_strings[i] = strings[i];
  }

  if (menu->_stringCount < menu->_visibleTextCount) {
    menu->_visibleTextCount = menu->_stringCount;
    menu->_upperFrameBound = menu->_visibleTextCount - 1;
  }

  // Font size may have changed. Readjust frame.
  COIMenuAdjustFrame(menu);
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
