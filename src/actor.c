#include "actor.h"

// For a 3 x 4 spritesheet
static int _spriteSheetOrderCols[] = { 0, 2, 1, 2 };

Actor* actorCreate(int actorType, COISprite* sprite,
		   int atk, int def, int agi, int hp, int tp, int sp) {
  Actor* actor = malloc(sizeof(Actor));
  actor->actorType = actorType;
  actor->sprite = sprite;
  COISpriteSetSheetIndex(actor->sprite, 2, 2);

  actor->atk = atk;
  actor->def = def;
  actor->agi = agi;
  actor->hp = hp;
  actor->tp = tp;
  actor->sp = sp;
  actor->hpMax = hp;
  actor->tpMax = tp;
  actor->spMax = sp;

  actor->techList = techCreateList(MAX_TECH_COUNT_NPC);

  actor->movementDirection = MOVING_NONE;
  actor->nextMovementDirection = MOVING_NONE;
  actor->_speed = 0;
  actor->_stepsLeft = 0;

  actor->_ticks = 0;
  actor->_spriteSheetColIndex = 0;
  actor->_spriteSheetRow = 0;

  return actor;
}

Actor* actorCreateOfType(int actorType, int x, int y, COIAssetLoader* loader, COIWindow* window) {
  COISprite* sprite;
  switch(actorType) {
  case ACTOR_SKELETON:
    sprite = COISpriteCreateFromAssetID(x, y, 64, 64, loader, 1, COIWindowGetRenderer(window));
    return actorCreate(actorType, sprite, 10, 5, 5, 3, 0, 0);
  }

  return NULL;
}

Actor* actorCreatePlayer(COISprite* sprite) {
  // Process for random stat generation, maybe specific for each class?
  Actor* actor = actorCreate(ACTOR_PLAYER, sprite, 15, 10, 8, 25, 10, 30);
  
  COISpriteSetSheetIndex(actor->sprite, 3, 0);

  actor->techList = techCreateList(MAX_TECH_COUNT_ALLY);
  techAddToList(actor->techList, TECH_ID_FOCUS);
  techAddToList(actor->techList, TECH_ID_COUNTER);
  techAddToList(actor->techList, TECH_ID_BRACE);
  techAddToList(actor->techList, TECH_ID_RAGE);
    

  return actor;
}

void actorDestroy(Actor* actor) {
  free(actor);
}

// COIBoard expects persistent sprites as a list of COISprite*
COISprite** actorGetSpriteList(Actor** actors, int numActors) {
  COISprite** sprites = malloc(sizeof(COISprite*) * numActors);
  for (int i = 0; i < numActors; i++) {
    sprites[i] = actors[i]->sprite;
  }

  return sprites;
}

char* actorGetNameFromType(int actorType) {
  switch (actorType) {
  case ACTOR_SKELETON:
    return "SKELETON";
  default:
    return "UNKNOWN";
  }
}

bool actorIsDead(Actor* actor) {
  return actor->hp == 0;
}

int _spriteSheetRowFromDirection(int xOffset, int yOffset) {
  // Moving left or right?
  if (xOffset != 0) {
    return xOffset > 0 ? 0 : 1;
  }

  // Moving up or down?
  if (yOffset != 0) {
    return yOffset > 0 ? 3 : 2;
  }

  return 0;
}

void actorMove(Actor* actor, int xOffset, int yOffset, COIBoard* board) {
  actor->_spriteSheetRow = _spriteSheetRowFromDirection(xOffset, yOffset);
  if (actor->_ticks >= ACTOR_SPRITE_TICKS) {
    actor->_spriteSheetColIndex = (actor->_spriteSheetColIndex + 1) % 4;
    actor->_ticks = 0;
  }
  int spriteSheetCol = _spriteSheetOrderCols[actor->_spriteSheetColIndex];
  COISpriteSetSheetIndex(actor->sprite, actor->_spriteSheetRow, spriteSheetCol);
  COIBoardMoveSprite(board, actor->sprite, xOffset, yOffset);
  actor->_ticks++;


}

// Actor continues current movement until stepsLeft = 0, then switches to
// nextMovementDirection
// No more setting MOVEMENT=NONE when reaching stepsLeft = 0
void actorQueueNextDirection(Actor* actor, int direction) {
  actor->nextMovementDirection = direction;
}



void actorStandStill(Actor* actor) {
  COISpriteSetSheetIndex(actor->sprite, actor->_spriteSheetRow, 2);
  //  actor->_speed = 0;
  //  actor->_stepsLeft = 0;
}

void actorFaceLeft(Actor* actor) {
  COISpriteSetSheetIndex(actor->sprite, 1, 2);
}

void actorFaceRight(Actor* actor) {
  COISpriteSetSheetIndex(actor->sprite, 0, 2);
}

void actorFaceUp(Actor* actor) {
  COISpriteSetSheetIndex(actor->sprite, 2, 2);
}

void actorFaceDown(Actor* actor) {
  COISpriteSetSheetIndex(actor->sprite, 3, 2);
}

void actorFaceDirection(Actor* actor, int direction) {
  switch (direction) {
  case MOVING_LEFT:
    actorFaceLeft(actor);
    break;
  case MOVING_RIGHT:
    actorFaceRight(actor);
    break;
  case MOVING_UP:
    actorFaceUp(actor);
    break;
  case MOVING_DOWN:
    actorFaceDown(actor);
    break;
  default:
    break;
  }
}

// Useful when resetting sprites in battle animations
void actorTurnAround(Actor* actor) {
  switch (actor->_spriteSheetRow) {
  case 0:
    actor->_spriteSheetRow = 1;
    break;
  case 1:
    actor->_spriteSheetRow = 0;
    break;
  case 2:
    actor->_spriteSheetRow = 3;
    break;
  case 3:
    actor->_spriteSheetRow = 2;
    break;
  default:
    actor->_spriteSheetRow = 0;
  }

  COISpriteSetSheetIndex(actor->sprite, actor->_spriteSheetRow, 2);
}
