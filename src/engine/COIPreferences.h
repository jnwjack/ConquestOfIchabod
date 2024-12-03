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
  LinkedList* list;
} MenuListComponent;

void MenuListComponentInit(MenuListComponent* component, COIBoard* board, COITextType* textType, COITextType* textTypeGray, char* label, int x, int y, LinkedList* list);
void MenuListComponentSetActive(MenuListComponent* component, bool active);

typedef struct MenuVolumeComponent {
  COIString* label;
  COIString* labelGray;
  COISprite* leftArrow;
  COISprite* rightArrow;
  COIRect *rects[VOLUME_BARS_COUNT];
} MenuVolumeComponent;

void MenuVolumeComponentInit(MenuVolumeComponent* component, COIBoard* board, COITextType* textType, COITextType* textTypeGray, char* label, int x, int y);
void MenuVolumeComponentSetActive(MenuVolumeComponent* component, bool active);

typedef struct MenuStringComponent {
  COIString* label;
  COIString* labelGray;
} MenuStringComponent;

void MenuStringComponentInit(MenuStringComponent* component, COIBoard* board, COITextType* textType, COITextType* textTypeGray, char* label, int x, int y);
void MenuStringComponentSetActive(MenuStringComponent* component, bool active);


typedef struct COIPreferencesMenu {
  COISprite* frame;
  MenuListComponent resolutionComponent;
  MenuListComponent fullscreenComponent;
  MenuVolumeComponent musicComponent;
  MenuVolumeComponent effectComponent;
  MenuStringComponent cancelComponent;
  MenuStringComponent applyComponent;
  unsigned int selectedComponent;
} COIPreferencesMenu;

void COIPreferencesMenuInit(COIPreferencesMenu* menu, COIBoard* board);
void COIPreferencesMenuProcessInput(COIPreferencesMenu* menu, int direction);

extern COIPreferences GLOBAL_PREFERENCES;

#endif
