#include "COIString.h"

COIChar* COICharCreate(char c, int x, int y, COITextType* textType) {
  COIChar* obj = malloc(sizeof(COIChar));

  obj->value = c;
  obj->x = x;
  obj->y = y;
  obj->drawRect = malloc(sizeof(SDL_Rect));
  obj->visible = true;
  obj->next = NULL;
  
  char str[2];
  str[0] = obj->value;
  str[1] = '\0';
  SDL_Surface* surface = TTF_RenderText_Solid(textType->font, (const char*)&str, textType->color);
  obj->texture = SDL_CreateTextureFromSurface(textType->renderer, surface);
  SDL_FreeSurface(surface);

  SDL_QueryTexture(obj->texture, NULL, NULL, &obj->w, &obj->h);
  obj->drawRect->x = x;
  obj->drawRect->y = y;
  obj->drawRect->w = obj->w;
  obj->drawRect->h = obj->h;

  return obj;
}

void COICharDestroy(COIChar* coiChar) {
  free(coiChar->drawRect);
  if (coiChar->next != NULL) {
    COICharDestroy(coiChar->next);
  }
  SDL_DestroyTexture(coiChar->texture);
  free(coiChar);
}

void COICharSetPos(COIChar* coiChar, int x, int y) {
  coiChar->x = x;
  coiChar->y = y;
  coiChar->drawRect->x = x;
  coiChar->drawRect->y = y;
}


COIString* COIStringCreate(char* string, int x, int y, COITextType* textType) {
  COIString* obj = malloc(sizeof(COIString));

  obj->fontSize = textType->fontSize;
  obj->visible = true;
  obj->index = -1;
  obj->x = x;
  obj->y = y;
  obj->yBottomLine = y; // Same as y until we request word wrapping
  obj->ticksPerChar = TYPING_TICKS_PER_CHAR;

  // Store string as a linked list of COIChars
  COIChar* current = NULL;
  COIChar* prev = NULL;
  int offsetX = x, offsetY = y;
  int i = 0;
  char c = string[i];
  while (c != '\0') {
    current = COICharCreate(c, offsetX, offsetY, textType);
    if (prev != NULL) {
      prev->next = current;
    } else {
      obj->_head = current;
    }

    // Set offsets to where we want to draw the next COIChar
    offsetX += current->w;

    prev = current;
    i++;
    c = string[i];
  }

  obj->_typingAnimationHead = obj->_head;
  obj->_typingAnimationTicks = 0;

  return obj;
}

void COIStringDestroy(COIString* obj) {
  COICharDestroy(obj->_head);
  free(obj);
}

void COIStringDraw(COIString* obj, SDL_Renderer* renderer, SDL_Rect* buf, float factorW, float factorH) {
  COIChar* current = obj->_head;
  while (current != NULL) {
    if (current->visible) {
      scaleRect(factorW, factorH, current->drawRect, buf);
      SDL_RenderCopy(renderer, current->texture, NULL, buf);
    }
    current = current->next;
  }
}

void COIStringSetPos(COIString* obj, int x, int y) {
  COIChar* current = obj->_head;
  int offsetX = x;
  obj->x = x;

  // Handle strings that already have multiple lines
  int offsetY = obj->yBottomLine - obj->y;
  obj->y = y;
  obj->yBottomLine = y + offsetY;
  while (current != NULL) {
    COICharSetPos(current, offsetX, y);
    offsetX += current->w;
    current = current->next;
  }
}

void COIStringSetY(COIString* obj, int y) {
  COIChar* current = obj->_head;

  // Handle strings that already have multiple lines
  int yDiff = y - obj->y;
  int offsetY = obj->yBottomLine - obj->y;
  obj->y = y;
  obj->yBottomLine = y + offsetY;
  while (current != NULL) {
    COICharSetPos(current, current->x, current->y + yDiff);
    current = current->next;
  }
}

void COIStringSetVisible(COIString* obj, bool visible) {
  COIChar* current = obj->_head;
  while (current != NULL) {
    current->visible = visible;
    current = current->next;
  }
  obj->visible = visible;
}

COIString** COIStringCopyList(COIString** src, int size) {
  COIString** copy = malloc(sizeof(COIString*) * size);
  for (int i = 0; i < size; i++) {
    copy[i] = src[i];
  }

  return copy;
}

// Return true when done
bool _confineToSpriteHelper(COIChar* coiChar, int x, int y, int xMax, COIString* string) {
  int xOffset = x;
  
  // Words are separated by a space.
  // If we decide to add a line break, do it by word. Move the whole
  // word down.
  COIChar* currentWordStart = coiChar;
  COIChar* current = coiChar;
  while (current != NULL) {
    COICharSetPos(current, xOffset, y);
    xOffset += current->w;
    if (current->value == ' ' || current->next == NULL) {
      if (xOffset > xMax) {
	string->yBottomLine = y + current->h;
	return _confineToSpriteHelper(currentWordStart, x, y + current->h, xMax, string);
      } else {
	currentWordStart = current->next;
      }
    }
    current = current->next;
  }

  return true;
}

void COIStringSetPosWithWrapping(COIString* obj, int x, int y, int width) {
  _confineToSpriteHelper(obj->_head, x, y, x + width, obj);
}

void COIStringConfineToSprite(COIString* obj, COISprite* sprite) {
  // Start at y of sprite, increase y (line breaks) in order to stay
  // inside of sprite
  int yOffset = sprite->_y + COI_PADDING;
  int xOffset = sprite->_x + COI_PADDING;
  int xMax = sprite->_x + (sprite->_width - COI_PADDING);

  obj->x = xOffset;
  obj->y = yOffset;
  obj->yBottomLine = yOffset;
  _confineToSpriteHelper(obj->_head, xOffset, yOffset, xMax, obj);
}

void COIStringPositionBelowString(COIString* below, COIString* above, bool onlySetY) {
  // Position string 'below' below string 'above'.
  // Use the y-position of 'above' as the base.
  // Use the x-position of 'above' as x for 'below'.

  if (onlySetY) {
    COIStringSetY(below, above->yBottomLine + above->fontSize + COI_PADDING);
  } else {
    COIStringSetPos(below, above->x, above->yBottomLine + above->fontSize + COI_PADDING);
  }
}

static int _findRightMostPixel(COIString* string) {
  COIChar* cur = string->_head;
  int rightmost = -1;
  while (cur != NULL) {
    if (cur->x + cur->w > rightmost) {
      rightmost = cur->x + cur->w;
    }
    cur = cur->next;
  }

  return rightmost;
}

void COIStringPositionRightOfString(COIString* right, COIString* left, int space) {
  // Position 'right' to the right of 'left' with 'space' pixels in between.

  // Need to find the rightmost pixel of 'left'. Add pixels of space after
  // this.
  int startingX = _findRightMostPixel(left) + space;
  COIStringSetPos(right, startingX, right->y);
}

// Continue the typing animation. If we're done, return true.
// When calling this, the string should start out invisible.
bool COIStringAnimateTyping(COIString* string) {
  if (string->_typingAnimationHead == NULL) {
    return true;
  }
  string->visible = true; // Regardless of where we are in animation, we say the string is visible
  string->_typingAnimationTicks++;
  if (string->_typingAnimationTicks >= string->ticksPerChar) {
    string->_typingAnimationHead->visible = true;
    string->_typingAnimationHead = string->_typingAnimationHead->next;
    string->_typingAnimationTicks = 0;
  }
  return false;
}

void COIStringSetStringCenter(COIString* string, int x, int y) {
  int rightmostX = -1;
  int bottommostY = -1;
  COIChar* current = string->_head;
  while (current != NULL) {
    int currentExtentX = current->x + current->w;
    int currentExtentY = current->y + current->h;
    if (currentExtentX > rightmostX) {
      rightmostX = currentExtentX;
    }
    if (currentExtentY > bottommostY) {
      bottommostY = currentExtentY;
    }
    current = current->next;
  }

  int width = rightmostX - string->x;
  int height = bottommostY - string->y;
  COIStringSetPos(string, x - width / 2, y - height / 2);
}
