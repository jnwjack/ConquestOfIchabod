#include "actor.h"

#include "special.h"

// For a 3 x 4 spritesheet
static int _spriteSheetOrderCols[] = { 0, 2, 1, 2 };

int actorModifiedStat(Stat* stat) {
  if (TimeStateInFuture(&stat->end)) {
    return stat->factor * stat->base;
  }
  return stat->base;
}

Actor* actorCreate(int actorType, COISprite* sprite,
		   int atk, int def, int agi, int hp, int tp, int sp) {
  Actor* actor = malloc(sizeof(Actor));
  actor->actorType = actorType;
  actor->sprite = sprite;
  COISpriteSetSheetIndex(actor->sprite, 2, 2);

  actor->atk.base = atk;
  actor->atk.factor = 1.0;
  TimeStateCopyGlobalTime(&actor->atk.end);
  
  actor->def.base = def;
  actor->def.factor = 1.0;
  TimeStateCopyGlobalTime(&actor->def.end);

  actor->agi.base = agi;
  actor->agi.factor = 1.0;
  TimeStateCopyGlobalTime(&actor->agi.end);

  actor->hp = hp;
  actor->tp = tp;
  actor->sp = sp;
  actor->hpMax = hp;
  actor->tpMax = tp;
  actor->spMax = sp;

  if (actor->actorType == ACTOR_PLAYER) {
    actor->techList = techCreateList(MAX_TECH_COUNT_ALLY);
  } else {
    actor->techList = techCreateList(MAX_TECH_COUNT_NPC);
  }

  IntListInitialize(&actor->specials, MAX_TECH_COUNT_ALLY);

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
    sprite = COISpriteCreateFromAssetID(x, y, 32, 32, loader, 29, COIWindowGetRenderer(window));
    return actorCreate(actorType, sprite, 10, 5, 5, 3, 0, 0);
  case ACTOR_TENTACLE:
    sprite = COISpriteCreateFromAssetID(x, y, 32, 32, loader, 30, COIWindowGetRenderer(window));
    {
      Actor* actor = actorCreate(actorType, sprite, 15, 10, 8, 25, 10, 30);
      IntListAdd(&actor->specials, SPECIAL_ID_CURSE);
      IntListAdd(&actor->specials, SPECIAL_ID_DRAIN_SPIRIT);
      IntListAdd(&actor->specials, SPECIAL_ID_REINFORCE);
      return actor;
    }
  case ACTOR_CHAGGAI:
    sprite = COISpriteCreateFromAssetID(x, y, 32, 32, loader, 32, COIWindowGetRenderer(window));
    return actorCreate(actorType, sprite, 10, 5, 5, 3, 0, 0);
  case ACTOR_LANDLORD:
    sprite = COISpriteCreateFromAssetID(x, y, 32, 32, loader, 32, COIWindowGetRenderer(window));
    return actorCreate(actorType, sprite, 10, 5, 5, 3, 0, 0);
  case ACTOR_MERCHANT:
    sprite = COISpriteCreateFromAssetID(x, y, 32, 32, loader, 32, COIWindowGetRenderer(window));
    return actorCreate(actorType, sprite, 10, 5, 5, 3, 0, 0);
  case ACTOR_BOOWOW:
    sprite = COISpriteCreateFromAssetID(x, y, 32, 32, loader, 31, COIWindowGetRenderer(window));
    {
      Actor* actor = actorCreate(actorType, sprite, 15, 9, 7, 12, 0, 5);
      IntListAdd(&actor->specials, SPECIAL_ID_HOWL);
      return actor;
    }
  case ACTOR_FEARWOLF:
    sprite = COISpriteCreateFromAssetID(x, y, 32, 32, loader, 1, COIWindowGetRenderer(window));
    return actorCreate(actorType, sprite, 28, 15, 12, 22, 0, 5);
  case ACTOR_WIRE_MOTHER:
    sprite = COISpriteCreateFromAssetID(x, y, 32, 32, loader, 28, COIWindowGetRenderer(window));
    {
      Actor* actor = actorCreate(actorType, sprite, 15, 9, 2, 12, 0, 75);
      IntListAdd(&actor->specials, SPECIAL_ID_HEAL);
      IntListAdd(&actor->specials, SPECIAL_ID_CURSE);
      IntListAdd(&actor->specials, SPECIAL_ID_DRAIN_SPIRIT);
      return actor;
    }
  case ACTOR_VOLCANETTE:
    sprite = COISpriteCreateFromAssetID(x, y, 32, 32, loader, 27, COIWindowGetRenderer(window));
    {
      Actor* actor = actorCreate(actorType, sprite, 15, 9, 10, 12, 0, 75);
      IntListAdd(&actor->specials, SPECIAL_ID_FIREBALL);
      IntListAdd(&actor->specials, SPECIAL_ID_ICE_SPEAR);
      IntListAdd(&actor->specials, SPECIAL_ID_CURSE);
      return actor;
    }
  default:
    printf("Invalid actor type when creating actor\n");
    return NULL;
  }
}

Actor* actorCreatePlayer(COISprite* sprite) {
  // Process for random stat generation, maybe specific for each class?
  Actor* actor = actorCreate(ACTOR_PLAYER, sprite, 15, 10, 50, 99, 10, 30);
  
  COISpriteSetSheetIndex(actor->sprite, 3, 0);
    
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
    return "MEATGRINDER";
  case ACTOR_TENTACLE:
    return "CORRUPTION";
  case ACTOR_BOOWOW:
    return "BOOWOW";
  case ACTOR_FEARWOLF:
    return "FEARWOLF";
  case ACTOR_WIRE_MOTHER:
    return "WIRE MOTHER";
  case ACTOR_VOLCANETTE:
    return "VOLCANETTE";
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
  actor->_spriteSheetRow = 1;
  
}

void actorFaceRight(Actor* actor) {
  COISpriteSetSheetIndex(actor->sprite, 0, 2);
  actor->_spriteSheetRow = 0;
}

void actorFaceUp(Actor* actor) {
  COISpriteSetSheetIndex(actor->sprite, 2, 2);
  actor->_spriteSheetRow = 2;
}

void actorFaceDown(Actor* actor) {
  COISpriteSetSheetIndex(actor->sprite, 3, 2);
  actor->_spriteSheetRow = 3;
}

// Face 'a' in the opposite direction that 'b' is facing.
// If 'a' and 'b' are adjacent, this makes 'a' and 'b' look like they're
// talking.
void actorMeetGaze(Actor* a, Actor* b) {
  switch (b->_spriteSheetRow) {
  case 0:
    actorFaceLeft(a);
    break;
  case 1:
    actorFaceRight(a);
    break;
  case 2:
    actorFaceDown(a);
    break;
  case 3:
    actorFaceUp(a);
    break;
  default:
    printf("Error. Invalid sprite sheet index.\n");
  }
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

int actorModifiedAtk(Actor* actor) {
  return actorModifiedStat(&actor->atk);
}

int actorModifiedDef(Actor* actor) {
  return actorModifiedStat(&actor->def);
}

int actorModifiedAgi(Actor* actor) {
  return actorModifiedStat(&actor->agi);
}

void actorUseConsumable(Actor* actor, Item* item) {
  Stat* modifier;
  
  switch (item->id) {
  case ITEM_ID_STRENGTH_POTION:
    modifier = &actor->atk;
    break;
  default:
    printf("Invalid consumable item.\n");
    modifier = NULL;
  }

  modifier->factor = item->strength / 10.0;
  TimeStateCopyGlobalTime(&modifier->end);
  TimeStateAddVal(&modifier->end, 2);
}
