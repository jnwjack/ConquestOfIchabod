#include "actor.h"

// For a 3 x 4 spritesheet
static int _spriteSheetOrderCols[] = { 0, 2, 1, 2 };

Actor* actorCreate(int actorType, COISprite* sprite,
		   int atk, int def, int agi, int hp, int tp, int sp) {
  Actor* actor = malloc(sizeof(Actor));
  actor->actorType = actorType;
  actor->sprite = sprite;

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

  actor->_ticks = 0;
  actor->_spriteSheetColIndex = 0;
  actor->_spriteSheetRow = 0;

  return actor;
}

Actor* actorCreateOfType(int actorType, int x, int y, COIAssetLoader* loader, COIWindow* window) {
  COISprite* sprite;
  switch(actorType) {
  case ACTOR_SKELETON:
    sprite = COISpriteCreateFromAssetID(x, y, 32, 32, loader, 7, COIWindowGetRenderer(window));
    return actorCreate(actorType, sprite, 10, 5, 5, 25, 0, 0);
  }

  return NULL;
}

Actor* actorCreatePlayer(COISprite* sprite) {
  Actor* actor = malloc(sizeof(Actor));
  actor->actorType = ACTOR_PLAYER;
  actor->sprite = sprite;
  COISpriteSetSheetIndex(actor->sprite, 3, 0);

  // Process for random stat generation, maybe specific for each class?
  actor->atk = 15;
  actor->def = 10;
  actor->agi = 8;
  actor->hp = 40;
  actor->tp = 10;
  actor->sp = 30;
  actor->hpMax = actor->hp;
  actor->tpMax = actor->tp;
  actor->spMax = actor->sp;

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

// COIBoard expects dynamic sprites as a list of COISprite*
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

// Should be called at beginning of input loop
void actorStandStill(Actor* actor) {
  COISpriteSetSheetIndex(actor->sprite, actor->_spriteSheetRow, 2);
}
