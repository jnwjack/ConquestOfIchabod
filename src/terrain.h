#ifndef TERRAIN_H
#define TERRAIN_H

// What kind of terrain are we on? Used when determining if we should have an encounter
typedef enum Terrain {
  TT_THICK_GRASS,
  TT_TENTACLE,
  TT_BROWN_GRASS,
  TT_THICK_GRASS_CORRUPT,
  TT_BROWN_GRASS_CORRUPT,
  TT_SAFE // No chance of battle
} Terrain;

#endif
