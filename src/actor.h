#ifndef ACTOR_H
#define ACTOR_H

#include "engine/COISprite.h"
#include "engine/COIAssetLoader.h"
#include "engine/COIWindow.h"
#include "tech.h"

#define ACTOR_SKELETON 0
#define ACTOR_PLAYER 1

// How many ticks per animation frame when moving
#define ACTOR_SPRITE_TICKS 10

#define MOVING_NONE 0
#define MOVING_LEFT 1
#define MOVING_RIGHT 2
#define MOVING_UP 3
#define MOVING_DOWN 4

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

  int _ticks;
  int _spriteSheetColIndex;
  int _spriteSheetRow;
} Actor;

Actor* actorCreate(int actorType, COISprite* sprite,
		   int atk, int def, int agi, int hp, int tp, int sp);
Actor* actorCreateOfType(int actorType, int x, int y, COIAssetLoader* loader, COIWindow* window);
Actor* actorCreatePlayer(COISprite* sprite);
void actorDestroy(Actor* actor);

// Create list of COISprites from list of actors
COISprite** actorGetSpriteList(Actor** actors, int numActors);

char* actorGetNameFromType(int actorType);
bool actorIsDead(Actor* actor);
void actorMove(Actor* actor, int xOffset, int yOffset, COIBoard* board);
void actorStandStill(Actor* actor);

#endif
