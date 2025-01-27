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
    COISpriteSetSheetIndex(sprite, 2, 2);
    return actorCreate(actorType, sprite, 34, 30, 15, 25, 0, 0);
  case ACTOR_MEAT_FLAYER:
    sprite = COISpriteCreateFromAssetID(x, y, 32, 32, loader, 94, COIWindowGetRenderer(window));
    COISpriteSetSheetIndex(sprite, 2, 2);
    return actorCreate(actorType, sprite, 65, 51, 36, 25, 0, 0);
  case ACTOR_TENTACLE:
    sprite = COISpriteCreateFromAssetID(x, y, 32, 32, loader, 30, COIWindowGetRenderer(window));
    COISpriteSetSheetIndex(sprite, 2, 2);
    {
      Actor* actor = actorCreate(actorType, sprite, 67, 55, 53, 45, 10, 30);
      IntListAdd(&actor->specials, SPECIAL_ID_CURSE);
      IntListAdd(&actor->specials, SPECIAL_ID_DRAIN_SPIRIT);
      IntListAdd(&actor->specials, SPECIAL_ID_REINFORCE);
      IntListAdd(&actor->specials, SPECIAL_ID_SILENCE);
      return actor;
    }
  case ACTOR_PARTY_GUY:
  case ACTOR_TIME_GUY:
  case ACTOR_NOSTALGIC_GUY:
  case ACTOR_TOWNS_GUY:
  case ACTOR_ENCOURAGEMENT_GUY:
  case ACTOR_HAVONVALE_GUY:
  case ACTOR_TAGNESSE_GUY:
  case ACTOR_TREE_GUY:
  case ACTOR_MERCHANT:
  case ACTOR_LANDLORD:
  case ACTOR_CHAGGAI:
  case ACTOR_CHESTS_GUY:
  case ACTOR_GEM_OF_TIME_GUY:
  case ACTOR_CLASS_GUY:
  case ACTOR_POTION_GUY:
  case ACTOR_FLEE_GUY:
  case ACTOR_MONSTER_COUNT_GUY:
    sprite = COISpriteCreateFromAssetID(x, y, 32, 32, loader, 32, COIWindowGetRenderer(window));
    COISpriteSetSheetIndex(sprite, 2, 2);
    return actorCreate(actorType, sprite, 10, 5, 5, 3, 0, 0);
  case ACTOR_BOOWOW:
    sprite = COISpriteCreateFromAssetID(x, y, 32, 32, loader, 31, COIWindowGetRenderer(window));
    COISpriteSetSheetIndex(sprite, 2, 2);
    {
      Actor* actor = actorCreate(actorType, sprite, 22, 24, 21, 15, 0, 5);
      IntListAdd(&actor->specials, SPECIAL_ID_HOWL);
      return actor;
    }
  case ACTOR_FEARWOLF:
    sprite = COISpriteCreateFromAssetID(x, y, 32, 32, loader, 93, COIWindowGetRenderer(window));
    COISpriteSetSheetIndex(sprite, 2, 2);
    return actorCreate(actorType, sprite, 45, 45, 36, 32, 0, 5);
  case ACTOR_WIRE_MOTHER:
    sprite = COISpriteCreateFromAssetID(x, y, 32, 32, loader, 28, COIWindowGetRenderer(window));
    COISpriteSetSheetIndex(sprite, 2, 2);
    {
      Actor* actor = actorCreate(actorType, sprite, 15, 30, 6, 25, 0, 75);
      IntListAdd(&actor->specials, SPECIAL_ID_MINOR_HEAL);
      IntListAdd(&actor->specials, SPECIAL_ID_CURSE);
      IntListAdd(&actor->specials, SPECIAL_ID_DRAIN_SPIRIT);
      return actor;
    }
  case ACTOR_WIRE_MAIDEN:
    sprite = COISpriteCreateFromAssetID(x, y, 32, 32, loader, 95, COIWindowGetRenderer(window));
    COISpriteSetSheetIndex(sprite, 2, 2);
    {
      Actor* actor = actorCreate(actorType, sprite, 30, 54, 21, 30, 0, 75);
      IntListAdd(&actor->specials, SPECIAL_ID_HEAL);
      IntListAdd(&actor->specials, SPECIAL_ID_CURSE);
      IntListAdd(&actor->specials, SPECIAL_ID_DRAIN_SPIRIT);
      return actor;
    } 
  case ACTOR_VOLCANETTE:
    sprite = COISpriteCreateFromAssetID(x, y, 32, 32, loader, 27, COIWindowGetRenderer(window));
    COISpriteSetSheetIndex(sprite, 2, 2);
    {
      Actor* actor = actorCreate(actorType, sprite, 21, 21, 40, 20, 0, 75);
      IntListAdd(&actor->specials, SPECIAL_ID_FIRE_BOLT);
      IntListAdd(&actor->specials, SPECIAL_ID_ICE_SPEAR);
      IntListAdd(&actor->specials, SPECIAL_ID_SILENCE);
      return actor;
    }
  case ACTOR_PYROID:
    sprite = COISpriteCreateFromAssetID(x, y, 32, 32, loader, 96, COIWindowGetRenderer(window));
    COISpriteSetSheetIndex(sprite, 2, 2);
    {
      Actor* actor = actorCreate(actorType, sprite, 42, 45, 75, 27, 0, 75);
      IntListAdd(&actor->specials, SPECIAL_ID_FIREBALL);
      // IntListAdd(&actor->specials, SPECIAL_ID_ICE_SPEAR);
      IntListAdd(&actor->specials, SPECIAL_ID_SILENCE);
      return actor;
    }    
  case ACTOR_CHEST:
    sprite = COISpriteCreateFromAssetID(x, y, 32, 32, loader, 68, COIWindowGetRenderer(window));
    return actorCreate(actorType, sprite, 0, 0, 0, 0, 0, 0);
  case ACTOR_CHEST_OPEN:
    sprite = COISpriteCreateFromAssetID(x, y, 32, 32, loader, 69, COIWindowGetRenderer(window));
    return actorCreate(actorType, sprite, 0, 0, 0, 0, 0, 0);
  default:
    printf("Invalid actor type when creating actor\n");
    return NULL;
  }
}

Actor* actorCreateTestPlayer(COISprite* sprite) {
  // Process for random stat generation, maybe specific for each class?
  Actor* actor = actorCreate(ACTOR_PLAYER, sprite, 999, 999, 999, 999, 25, 15);
  for (int i = 0; i < 21; i++) {
    IntListAdd(&actor->specials, i);
  }

  for (int i = 0; i < 8; i++) {
    techAddToList(actor->techList, i);
  }

  COISpriteSetSheetIndex(actor->sprite, 1, 2);
    
  return actor;
}

Actor* actorCreatePlayer(COISprite* sprite) {
  // Process for random stat generation, maybe specific for each class?
  Actor* actor = actorCreate(ACTOR_PLAYER, sprite, 24, 30, 24, 50, 25, 15);
  
  COISpriteSetSheetIndex(actor->sprite, 1, 2);
    
  return actor;
}

// Actor* actorCreateTEMPPlayer(COISprite* sprite) {
//   Actor* actor = actorCreate(ACTOR_PLAYER, sprite, , 30, 24, 50, 25, 15);

//   COISpriteSetSheetIndex(actor->sprite, 1, 2);
// }


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
  case ACTOR_MEAT_FLAYER:
    return "MEATFLAYER";
  case ACTOR_TENTACLE:
    return "CORRUPTION";
  case ACTOR_BOOWOW:
    return "BOOWOW";
  case ACTOR_FEARWOLF:
    return "FEARWOLF";
  case ACTOR_WIRE_MOTHER:
    return "WIRE MOTHER";
  case ACTOR_WIRE_MAIDEN:
    return "WIRE MAIDEN";
  case ACTOR_VOLCANETTE:
    return "VOLCANETTE";
  case ACTOR_PYROID:
    return "PYROID";
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
  if (COISpriteHasMultipleFrames(a->sprite)) {
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
    modifier->factor = item->strength / 10.0;
    TimeStateCopyGlobalTime(&modifier->end);
    TimeStateAddVal(&modifier->end, 2);
    break;
  case ITEM_ID_DEF_SCROLL:
    modifier = &actor->def;
    modifier->factor = item->strength / 10.0;
    TimeStateCopyGlobalTime(&modifier->end);
    TimeStateAddVal(&modifier->end, 2);
    break;
  case ITEM_ID_AGI_SCROLL:
    modifier = &actor->agi;
    modifier->factor = item->strength / 10.0;
    TimeStateCopyGlobalTime(&modifier->end);
    TimeStateAddVal(&modifier->end, 2);
    break;
  case ITEM_ID_HEALING_POTION:
    actor->hp = MIN(actor->hpMax, actor->hp + item->strength);
    break;
  case ITEM_ID_TP_POTION:
    actor->tp = MIN(actor->tpMax, actor->tp + item->strength);
    break;
  case ITEM_ID_SP_POTION:
    actor->sp = MIN(actor->spMax, actor->sp + item->strength);
    break;
  case ITEM_ID_MOUNTAIN_JUICE:
    actor->hp = MIN(actor->hpMax, actor->hp + item->strength);
    actor->tp = MIN(actor->tpMax, actor->tp + item->strength);
    actor->sp = MIN(actor->spMax, actor->sp + item->strength);
    break;
  default:
    printf("Invalid consumable item.\n");
    modifier = NULL;
  }

}

// Used when changing player sprite
void actorChangeSprite(Actor* actor, int spriteID) {
  if (actor->sprite != NULL) {
    COISprite* newSprite = COISpriteCreateFromAssetID(actor->sprite->_x, actor->sprite->_y, 32, 32, COI_GLOBAL_LOADER, spriteID, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
    int spriteSheetCol = _spriteSheetOrderCols[actor->_spriteSheetColIndex];
    // COISpriteSetSheetIndex(newSprite, actor->_spriteSheetRow, spriteSheetCol);
    COISpriteSetSheetIndex(newSprite, 2, 2);
    COISpriteDestroy(actor->sprite);
    actor->sprite = newSprite;
  }
}
