#ifndef ACTOR_H
#define ACTOR_H

#include "engine/COISprite.h"
#include "engine/COIAssetLoader.h"
#include "engine/COIWindow.h"
#include "tech.h"

#define ACTOR_SKELETON 0
#define ACTOR_PLAYER 1

// Holds data about a character in the world
// Sprite, stats, walking behaviors, etc.

// NPCs, enemies, etc. are Actors

// Sprites of actors are the dynamic sprites in COIBoard
typedef struct Actor {
  int actorType;
  COISprite* sprite;

  // Battle statistics
  int atk;
  int def;
  int agi;
  int hp;
  int tp; // For tech
  int sp; // For special
  int hpMax;
  int tpMax;
  int spMax;

  TechList* techList;
} Actor;

Actor* actorCreate(int actorType, COISprite* sprite,
		   int atk, int def, int agi, int hp, int tp, int sp);
Actor* actorCreateOfType(int actorType, int x, int y, COIAssetLoader* loader, COIWindow* window);
Actor* actorCreatePlayer(COISprite* sprite);
void actorDestroy(Actor* actor);

// Create list of COISprites from list of actors
COISprite** actorGetSpriteList(Actor** actors, int numActors);

char* actorGetNameFromType(int actorType);

#endif
