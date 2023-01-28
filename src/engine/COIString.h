#ifndef COIMENU_H
#define COIMENU_H

#include "COIChar.h"

typedef struct COIString {
  COIChar** _chars;
  
  int x;
  int y;
  bool visible;
}COIString;

COIString* COIStringCreate(char* string);

#endif
