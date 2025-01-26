#ifndef PREFERENCES_H
#define PREFERENCES_H

#include "COIMenu.h"
#include "COIBoard.h"

#define VOLUME_BARS_COUNT 8

#define COMPONENT_INDEX_RESOLUTION 0
#define COMPONENT_INDEX_FULLSCREEN 1
#define COMPONENT_INDEX_MUSIC 2
#define COMPONENT_INDEX_EFFECT 3
#define COMPONENT_INDEX_CONFIRM 4

struct CircleListNode;
typedef struct CircleListNode {
  void* data;
  struct CircleListNode* prev;
  struct CircleListNode* next;
  int index;
} CircleListNode;

typedef struct CircleList {
   CircleListNode* current;
   LinkedList* list;
}CircleList;

typedef struct COIResolution {
  unsigned int width;
  unsigned int height;
} COIResolution;

typedef struct COIPreferences {
  bool fullscreen;
  unsigned int musicVolume;
  unsigned int effectVolume;
  COIResolution resolution;
} COIPreferences;

void COIPreferencesInit();
void COIPreferencesWriteToFile();

typedef struct MenuListComponent {
  COIString* label;
  COIString* labelGray;
  COISprite* leftArrow;
  COISprite* rightArrow;
  CircleList circleList;
  LinkedList* list;
} MenuListComponent;

typedef struct MenuVolumeComponent {
  COIString* label;
  COIString* labelGray;
  COISprite* leftArrow;
  COISprite* rightArrow;
  COIRect *rects[VOLUME_BARS_COUNT];
} MenuVolumeComponent;

typedef struct MenuStringComponent {
  COIString* label;
  COIString* labelGray;
} MenuStringComponent;

typedef struct COIPreferencesMenu {
  COISprite* frame;
  MenuListComponent resolutionComponent;
  MenuListComponent fullscreenComponent;
  MenuVolumeComponent musicComponent;
  MenuVolumeComponent effectComponent;
  MenuStringComponent cancelComponent;
  MenuStringComponent applyComponent;
  unsigned int selectedComponent;

  unsigned int oldMusicValue;
  unsigned int oldEffectValue;
  bool modifiedGraphics;
} COIPreferencesMenu;

void COIPreferencesMenuInit(COIPreferencesMenu* menu, COIBoard* board);
void COIPreferencesMenuSetVisible(COIPreferencesMenu* menu, bool visible);
void COIPreferencesMenuProcessInput(COIPreferencesMenu* menu, int direction);
bool COIPreferenecesPrefDataExists();

extern COIPreferences GLOBAL_PREFERENCES;

#endif
