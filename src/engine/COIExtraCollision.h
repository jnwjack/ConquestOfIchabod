#ifndef COIEXTRACOLLISION_H
#define COIEXTRACOLLISION_H


// Collision constants - general
#define COI_NO_COLLISION 0
#define COI_COLLISION 1

// Collision constants - extra collisions
#define ARMORY_DOOR 2
#define BATTLE 3
#define THICK_GRASS 4
#define RENTABLE_HOUSE_DOOR 5
#define TENTACLE 6
#define SHOP_DOOR 7
#define POTION_SHOP_DOOR 8
#define BROWN_GRASS 9
#define THICK_GRASS_CORRUPT 10
#define BROWN_GRASS_CORRUPT 11

typedef struct COIExtraCollision {
  int tlX;
  int tlY;
  int brX;
  int brY;
  int returnValue;
}COIExtraCollision;

#endif
