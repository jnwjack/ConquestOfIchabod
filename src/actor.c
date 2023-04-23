#include "actor.h"

Actor* actorCreate(int actorType, COISprite* sprite) {
  Actor* actor = malloc(sizeof(Actor));
  actor->actorType = actorType;
  actor->sprite = sprite;

  return actor;
}

Actor* actorCreateOfType(int actorType, int x, int y, COIAssetLoader* loader, COIWindow* window) {

  COISprite* sprite;
  switch(actorType) {
  case ACTOR_SKELETON:
    sprite = COISpriteCreateFromAssetID(x, y, 32, 32, loader, 6, COIWindowGetRenderer(window));
    return actorCreate(actorType, sprite);
  }

  return NULL;
}

// COIBoard expects dynamic sprites as a list of COISprite*
COISprite** actorGetSpriteList(Actor** actors, int numActors) {
  COISprite** sprites = malloc(sizeof(COISprite*));
  for (int i = 0; i < numActors; i++) {
    sprites[i] = actors[i]->sprite;
  }

  return sprites;
}
