#ifndef ACTOR_H
#define ACTOR_H

#include "engine/COISprite.h"
#include "engine/COIAssetLoader.h"
#include "engine/COIWindow.h"

#define ACTOR_SKELETON 0


// Holds data about a character in the world
// Sprite, stats, walking behaviors, etc.

// NPCs, enemies, etc. are Actors

// Sprites of actors are the dynamic sprites in COIBoard
typedef struct Actor {
  int actorType;
  COISprite* sprite;
} Actor;

Actor* actorCreate(int actorType, COISprite* sprite);
Actor* actorCreateOfType(int actorType, int x, int y, COIAssetLoader* loader, COIWindow* window);

// Create list of COISprites from list of actors
COISprite** actorGetSpriteList(Actor** actors, int numActors);

#endif
