#include "COIMenu.h"

COIMenu* COIMenuCreate(COITextGroup* group,
		       COISprite* frame,
		       COISprite* pointer) {
  COIMenu* menu = malloc(sizeof(COIMenu));
  menu->_group = group;
  menu->_frame = frame;
  menu->_pointer = pointer;
  menu->_x = frame->_x;
  menu->_y = frame->_y;
  menu->_width = frame->_width;
  menu->_height = frame->_height;
  menu->_textsCount = 0;
  menu->_texts = NULL;
  menu->_current = 0;
  menu->_visibleTextCount = (menu->_height - COI_MENU_OFFSET_Y) / (group->_fontSize + COI_MENU_PADDING);
  menu->_lowerFrameBound = 0;
  menu->_upperFrameBound = menu->_visibleTextCount - 1;
  // Can't have upper bound greater than lower
  if (menu->_lowerFrameBound > menu->_upperFrameBound) {
    printf("Error when creating menu\n");
    free(menu);
    return NULL;
  }

  /*menu->_frame->_x = x;
    menu->_frame->_y = y;*/
  menu->_frame->_autoHandle = false;
  menu->_pointer->_autoHandle = false;

  return menu;
}

void COIMenuDestroy(COIMenu* menu) {
  if (menu == NULL) {
    return;
  }
  if (menu->_texts != NULL) {
    free(menu->_texts);
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
  COIText* text;
  for (i = 0; i < menu->_textsCount; i++) {
    text = menu->_texts[i];

    if (i >= menu->_lowerFrameBound && i <= menu->_upperFrameBound) {
      text->_visible = true;
      COITextSetPos(text,
		    menu->_x + COI_MENU_OFFSET_X,
		    menu->_y + COI_MENU_OFFSET_Y + (slot * (menu->_group->_fontSize + COI_MENU_PADDING)));
		    //text->_x = menu->_x + COI_MENU_OFFSET_X;
		    //text->_y = menu->_y + COI_MENU_OFFSET_Y + (slot * menu->_group->_fontSize);
      slot++;
    } else {
      text->_visible = false;
    }
  }
}

// Sets all texts false
void COIMenuSetInvisible(COIMenu* menu) {
  menu->_frame->_visible = false;
  menu->_pointer->_visible = false;
  int i;
  COIText* text;
  for (i = 0; i < menu->_textsCount; i++) {
    text = menu->_texts[i];

    text->_visible = false;
  }
}

void COIMenuSetTexts(COIMenu* menu, int indices[], int numIndices) {
  if (menu->_texts != NULL) {
    free(menu->_texts);
  }
  menu->_texts = malloc(numIndices * sizeof(COIText*));
  menu->_textsCount= numIndices;
  
  if (menu->_textsCount < menu->_visibleTextCount) {
    menu->_visibleTextCount = menu->_textsCount;
    menu->_upperFrameBound = menu->_visibleTextCount - 1;
    
  }
  COIText** texts = COITextGroupGetTexts(menu->_group);
  int i;
  for (i = 0; i < numIndices; i++) {
    menu->_texts[i] = texts[indices[i]];
  }
}

void COIMenuIncrement(COIMenu* menu, int step) {
  
  int nextCurrent = menu->_current + step;
  if (nextCurrent >= menu->_textsCount) {
    menu->_current = 0;
  } else if (nextCurrent < 0) {
    menu->_current = menu->_textsCount - 1;
  } else {
    menu->_current = nextCurrent;
  }

  //nextCurrent = nextCurrent < 0 ? menu->_textsCount + step : nextCurrent % menu->_textsCount;

  //menu->_current = nextCurrent;
  
  if (menu->_current > menu->_upperFrameBound) {
    menu->_upperFrameBound = menu->_current;
    menu->_lowerFrameBound = menu->_current - (menu->_visibleTextCount - 1);
    COIMenuSetVisible(menu);
  } else if (menu->_current < menu->_lowerFrameBound)  {
    menu->_lowerFrameBound = menu->_current;
    menu->_upperFrameBound = menu->_current + (menu->_visibleTextCount - 1);
    COIMenuSetVisible(menu);
  }

  int pointerLocation = menu->_current - menu->_lowerFrameBound;
  menu->_pointer->_y = menu->_y + COI_MENU_OFFSET_Y + (pointerLocation * menu->_pointer->_height);
}
