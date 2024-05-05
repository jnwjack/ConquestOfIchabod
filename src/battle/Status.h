#ifndef ALLYSTATUS_H
#define ALLYSTATUS_H

#include "../actor.h"

// UI component that displays an ally's
// HP, TP, SP, and conditions.
typedef struct AllyStatus {
  COITextType* textType;
  COIBoard* board;
  COISprite* frame;
  COIString* hp;
  COIString* tp;
  COIString* sp;

  // Used for checking if we actually need to update
  int _hpVal;
  int _tpVal;
  int _spVal;
} AllyStatus;

AllyStatus* AllyStatusCreate(COIBoard* board, COIWindow* window, int fontSize);
void AllyStatusUpdate(AllyStatus* status, Actor* actor);
void AllyStatusDestroy(AllyStatus* status);

#endif
