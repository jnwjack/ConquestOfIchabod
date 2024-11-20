#ifndef ACTOR_H
#define ACTOR_H

#include "engine/COI.h"
#include "TimeState.h"
#include "tech.h"
#include "items.h"

#define ACTOR_NONE -1
#define ACTOR_SKELETON 0
#define ACTOR_PLAYER 1
#define ACTOR_CHAGGAI 2
#define ACTOR_TENTACLE 3
#define ACTOR_LANDLORD 4
#define ACTOR_MERCHANT 5
#define ACTOR_BOOWOW 6
#define ACTOR_FEARWOLF 7
#define ACTOR_WIRE_MOTHER 8
#define ACTOR_VOLCANETTE 9
#define ACTOR_CHEST 10
#define ACTOR_CHEST_OPEN 11
#define ACTOR_TAGNESSE_GUY 12
#define ACTOR_TREE_GUY 13
#define ACTOR_HAVONVALE_GUY 14
#define ACTOR_WIRE_MAIDEN 15
#define ACTOR_MEAT_FLAYER 16
#define ACTOR_PYROID 17
#define ACTOR_CHESTS_GUY 18

// Buff values
#define ACTOR_BUFF_PARRYING 1

// How many ticks per animation frame when moving
#define ACTOR_SPRITE_TICKS 8

// Represents the effect of buffs/debuffs on a single stat.
// Caused by spells, potions, etc.
typedef struct Stat {
  int base;
  float factor;  // Modifier factor, affected by buffs/debuffs, etc.
  TimeState end; // Point at which modifier effect is over
} Stat;

// Holds data about a character in the world
// Sprite, stats, walking behaviors, etc.

// NPCs, enemies, etc. are Actors

// Sprites of actors are the dynamic sprites in COIBoard
typedef struct Actor {
  int actorType;
  COISprite* sprite;

  // Battle statistics
  Stat atk;
  Stat def;
  Stat agi;
  int hp;
  int tp; // For tech
  int sp; // For special
  int hpMax;
  int tpMax;
  int spMax;

  TechList* techList;
  IntList specials;

  int movementDirection;
  int nextMovementDirection;
  int _stepsLeft; // We want to move in fixed chunks. We're on a grid
  int _speed;

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
//void actorStartMovement(Actor* actor, int direction, int speed);
void actorQueueMovement(Actor* actor, int direction, int speed);
bool actorContinueMovement(Actor* actor, COIBoard* board);
void actorStandStill(Actor* actor);
void actorFaceLeft(Actor* actor);
void actorFaceRight(Actor* actor);
void actorFaceUp(Actor* actor);
void actorFaceDown(Actor* actor);
void actorMeetGaze(Actor* a, Actor* b);
void actorFaceDirection(Actor* actor, int direction);
void actorTurnAround(Actor* actor);
int actorModifiedStat(Stat* stat);
int actorModifiedAtk(Actor* actor);
int actorModifiedDef(Actor* actor);
int actorModifiedAgi(Actor* actor);
void actorUseConsumable(Actor* actor, Item* item);
void actorChangeSprite(Actor* actor, int spriteID);

#endif
