#include "COIPreferences.h"
#include "COIWindow.h"

COIPreferences GLOBAL_PREFERENCES;

#define RESOLUTION_COUNT  10
static COIResolution resolutions[RESOLUTION_COUNT] = { { 640, 480 }, { 720, 576 }, { 800, 600 }, { 1024, 768 }, { 1280, 720 }, { 1280, 960 }, { 1440, 1080 }, { 1600, 1200 }, {1920, 1080 }, { 2880, 1800 } };

#define VOLUME_BAR_MAX_HEIGHT 32

static void _resolutionToString(COIResolution resolution, char* buf, size_t bufSize) {
  snprintf(buf, bufSize, "%ix%i", resolution.width, resolution.height);
}

static void CircleListInit(CircleList* circleList, LinkedList* baseList) {
  circleList->list = baseList;
  LinkedListResetCursor(baseList);
  void* data = LinkedListNext(baseList);
  int i = 0;
  CircleListNode* prev = NULL;
  CircleListNode* first = NULL;
  while (data) {
    circleList->current = malloc(sizeof(CircleListNode));
    circleList->current->index = i++;
    circleList->current->data = data;
    if (prev) {
      prev->next = circleList->current;
      circleList->current->prev = prev;
    } else {
      first = circleList->current; 
    }
    prev = circleList->current;

    data = LinkedListNext(baseList);
  }

  if (first) {
    first->prev = circleList->current;
    circleList->current->next = first;
    circleList->current = first;
  }
}

void COIPreferencesInit() {
  char* prefFilename = getPreferencesFileLocation();
  FILE* fp = fopen(prefFilename, "r");

  char* line = NULL;
  size_t len = 0;

  // Resolution and fullscreen
  getline(&line, &len, fp);
  GLOBAL_PREFERENCES.resolution.width = SDL_atoi(line);
  getline(&line, &len, fp);
  GLOBAL_PREFERENCES.resolution.height = SDL_atoi(line);
  getline(&line, &len, fp);
  GLOBAL_PREFERENCES.fullscreen = SDL_atoi(line) > 0;

  // Sound
  getline(&line, &len, fp);
  GLOBAL_PREFERENCES.effectVolume = SDL_atoi(line);
  getline(&line, &len, fp);
  GLOBAL_PREFERENCES.musicVolume = SDL_atoi(line);

  fclose(fp);
  free(prefFilename);
  if (line) {
    free(line);
  }
}

void COIPreferencesWriteToFile() {
  char* prefFilename = getPreferencesFileLocation();
  FILE* fp = fopen(prefFilename, "w");

  fprintf(fp, "%u\n", GLOBAL_PREFERENCES.resolution.width);
  fprintf(fp, "%u\n", GLOBAL_PREFERENCES.resolution.height);
  fprintf(fp, "%u\n", GLOBAL_PREFERENCES.fullscreen);
  fprintf(fp, "%u\n", GLOBAL_PREFERENCES.effectVolume);
  fprintf(fp, "%u\n", GLOBAL_PREFERENCES.musicVolume);

  fclose(fp);
  free(prefFilename);
}

static void MenuListComponentInit(MenuListComponent* component, COIBoard* board, COITextType* textType, COITextType* textTypeGray, char* label, int x, int y, LinkedList* list) {
  component->label = COIStringCreate(label, x, 0, textType);
  COIBoardAddString(board, component->label, 1);
  COIStringSetVisible(component->label, false);
  component->labelGray = COIStringCreate(label, x, 0, textTypeGray);
  COIBoardAddString(board, component->labelGray, 1);

  component->leftArrow = COISpriteCreateFromAssetID(x + 160, y, 32, 32, COI_GLOBAL_LOADER, 46, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  component->rightArrow = COISpriteCreateFromAssetID(x + 343, y, 32, 32, COI_GLOBAL_LOADER, 47, COIWindowGetRenderer(COI_GLOBAL_WINDOW));

  int textCenterY = component->leftArrow->_y + component->leftArrow->_height / 2;
  int listItemsCenterX = component->leftArrow->_x + 32 + (component->rightArrow->_x - (component->leftArrow->_x + 32)) / 2;

  COIStringSetStringCenter(component->label, 0, textCenterY);
  COIStringSetPos(component->label, x, component->label->y);
  COIStringSetStringCenter(component->labelGray, 0, textCenterY);
  COIStringSetPos(component->labelGray, x, component->labelGray->y);

  component->leftArrow->_autoHandle = false;
  component->leftArrow->_visible = false;
  component->rightArrow->_autoHandle = false;
  component->rightArrow->_visible = false;
  COIBoardAddDynamicSprite(board, component->leftArrow, 1);
  COIBoardAddDynamicSprite(board, component->rightArrow, 1);

  component->list = list;
  LinkedListResetCursor(component->list);
  COIString* string = LinkedListNext(component->list);
  bool visible = true;
  while (string) {
    COIBoardAddString(board, string, 1);
    COIStringSetStringCenter(string, listItemsCenterX, textCenterY);
    COIStringSetVisible(string, visible);
    visible = false;

    string = LinkedListNext(component->list);
  }

  CircleListInit(&component->circleList, list);
}

static void MenuListComponentSetActive(MenuListComponent* component, bool active) {
  COIStringSetVisible(component->label, active);
  COIStringSetVisible(component->labelGray, !active);
  component->leftArrow->_visible = active;
  component->rightArrow->_visible = active;
}

static void MenuListComponentNext(MenuListComponent* component, int val) {
  COIString* oldString = (COIString*)component->circleList.current->data;
  if (val > 0) {
    component->circleList.current = component->circleList.current->next;
  } else {
    component->circleList.current = component->circleList.current->prev;
  }
  
  COIStringSetVisible(oldString, false);
  COIStringSetVisible((COIString*)component->circleList.current->data, true);
}

static void MenuListComponentSetVisible(MenuListComponent* component, bool visible) {
  COIStringSetVisible(component->label, visible);
  COIStringSetVisible(component->labelGray, visible);
  component->leftArrow->_visible = visible;
  component->rightArrow->_visible = visible;
  COIStringSetVisible((COIString*)component->circleList.current->data, visible);
}

static void MenuListComponentSetCurrentString(MenuListComponent* component, int index) {
  COIStringSetVisible((COIString*)component->circleList.current->data, false);
  while (component->circleList.current->index != index) {
    component->circleList.current = component->circleList.current->next;
  }
  COIStringSetVisible((COIString*)component->circleList.current->data, true);
}

static void MenuVolumeComponentInit(MenuVolumeComponent* component, COIBoard* board, COITextType* textType, COITextType* textTypeGray, char* label, int x, int y) {
  component->label = COIStringCreate(label, x, 0, textType);
  COIBoardAddString(board, component->label, 1);
  COIStringSetVisible(component->label, false);
  component->labelGray = COIStringCreate(label, x, 0, textTypeGray);
  COIBoardAddString(board, component->labelGray, 1);

  component->leftArrow = COISpriteCreateFromAssetID(x + 160, y, 32, 32, COI_GLOBAL_LOADER, 46, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  component->rightArrow = COISpriteCreateFromAssetID(x + 343, y, 32, 32, COI_GLOBAL_LOADER, 47, COIWindowGetRenderer(COI_GLOBAL_WINDOW));

  int textCenterY = component->leftArrow->_y + component->leftArrow->_height / 2;
  int listItemsCenterX = component->leftArrow->_x + 32 + (component->rightArrow->_x - (component->leftArrow->_x + 32)) / 2;

  COIStringSetStringCenter(component->label, 0, textCenterY);
  COIStringSetPos(component->label, x, component->label->y);
  COIStringSetStringCenter(component->labelGray, 0, textCenterY);
  COIStringSetPos(component->labelGray, x, component->labelGray->y);

  component->leftArrow->_autoHandle = false;
  component->leftArrow->_visible = false;
  component->rightArrow->_autoHandle = false;
  component->rightArrow->_visible = false;
  COIBoardAddDynamicSprite(board, component->leftArrow, 1);
  COIBoardAddDynamicSprite(board, component->rightArrow, 1);

  int baseX = component->leftArrow->_x + component->leftArrow->_width;
  int bottomY = component->leftArrow->_y + component->leftArrow->_height;
  int width = component->rightArrow->_x - baseX;
  int paddingBetweenBars = 1;
  int widthPerBar = (width - paddingBetweenBars * (VOLUME_BARS_COUNT - 1)) / VOLUME_BARS_COUNT;
  int heightIncrease = VOLUME_BAR_MAX_HEIGHT / (VOLUME_BARS_COUNT);
  int height = heightIncrease;
  for (int i = 0; i < VOLUME_BARS_COUNT; i++) {
    height += heightIncrease;
    COIBoardAddRect(board, baseX + (widthPerBar + paddingBetweenBars) * i, bottomY - height, widthPerBar, height, 255, 255, 255, 255, true, 1);
    component->rects[i] = &board->drawLayers[1].rects[board->drawLayers[1].numRects - 1];
  }
}

static void MenuVolumeComponentSetActive(MenuVolumeComponent* component, bool active) {
  COIStringSetVisible(component->label, active);
  COIStringSetVisible(component->labelGray, !active);
  component->leftArrow->_visible = active;
  component->rightArrow->_visible = active;
}

static void MenuVolumeComponentUpdateBarVisibility(MenuVolumeComponent* component, unsigned int val) {
  unsigned int highestBar = val / (COI_SOUND_MAX_VOLUME / VOLUME_BARS_COUNT);
  for (unsigned int i = 0; i < VOLUME_BARS_COUNT; i++) {
    if (i < highestBar) {
      component->rects[i]->r = 255;
      component->rects[i]->g = 255;
      component->rects[i]->b = 255;
    } else {
      component->rects[i]->r = 120;
      component->rects[i]->g = 120;
      component->rects[i]->b = 120;
    }
  }
}

static void MenuVolumeComponentSetVisible(MenuVolumeComponent* component, bool visible) {
  COIStringSetVisible(component->label, visible);
  COIStringSetVisible(component->labelGray, visible);
  component->leftArrow->_visible = visible;
  component->rightArrow->_visible = visible;
  for (unsigned int i = 0; i < VOLUME_BARS_COUNT; i++) {
    component->rects[i]->visible = visible;
  }
}

static void MenuStringComponentInit(MenuStringComponent* component, COIBoard* board, COITextType* textType, COITextType* textTypeGray, char* label, int x, int y) {
  component->label = COIStringCreate(label, x, y, textType);
  COIBoardAddString(board, component->label, 1);
  COIStringSetVisible(component->label, false);
  component->labelGray = COIStringCreate(label, x, y, textTypeGray);
  COIBoardAddString(board, component->labelGray, 1);
  COIStringSetVisible(component->labelGray, true);
}

static void MenuStringComponentSetActive(MenuStringComponent* component, bool active) {
  COIStringSetVisible(component->label, active);
  COIStringSetVisible(component->labelGray, !active);
}

static bool MenuStringComponentIsActive(MenuStringComponent* component) {
  return component->label->visible;
}

static void MenuStringComponentSetVisible(MenuStringComponent* component, bool visible) {
  COIStringSetVisible(component->label, visible);
  COIStringSetVisible(component->labelGray, visible);
}

void COIPreferencesMenuInit(COIPreferencesMenu* menu, COIBoard* board) {
  int padding = 20;
  int width = 400;
  int height = 300;

  int ySpaceBetweenComponents = (height - (padding * 2)) / 5;

  COITextType* textType = COITextTypeCreate(14, 255, 255, 255, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COITextType* textTypeGray = COITextTypeCreate(14, 120, 120, 120, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  COISprite *leftArrow, *rightArrow;
  menu->frame = COISpriteCreateFromAssetID(120, 90, width, height, COI_GLOBAL_LOADER, 5, COIWindowGetRenderer(COI_GLOBAL_WINDOW));
  menu->frame->_autoHandle = false;
  menu->frame->_visible = true;
  menu->modifiedGraphics = false;
  COIBoardAddDynamicSprite(board, menu->frame, 1);

  LinkedList *list = LinkedListCreate();
  char buf[MAX_STRING_SIZE];
  for (int i = 0; i < RESOLUTION_COUNT; i++) {
    _resolutionToString(resolutions[i], buf, MAX_STRING_SIZE);
    LinkedListAdd(list, COIStringCreate(buf, 0, 0, textType));
  }
  MenuListComponentInit(&menu->resolutionComponent, board, textType, textTypeGray, "Resolution", menu->frame->_x + padding, menu->frame->_y + padding, list);

  for (int i = 0; i < RESOLUTION_COUNT; i++) {
    if (resolutions[i].width == GLOBAL_PREFERENCES.resolution.width && resolutions[i].height == GLOBAL_PREFERENCES.resolution.height) {
      MenuListComponentSetCurrentString(&menu->resolutionComponent, i);
      break;
    }
  }

  list = LinkedListCreate();
  LinkedListAdd(list, COIStringCreate("Off", 0, 0, textType));
  LinkedListAdd(list, COIStringCreate("On", 0, 0, textType));
  MenuListComponentInit(&menu->fullscreenComponent, board, textType, textTypeGray, "Fullscreen", menu->frame->_x + padding, menu->frame->_y + padding + ySpaceBetweenComponents, list);
  if (GLOBAL_PREFERENCES.fullscreen) {
    MenuListComponentSetCurrentString(&menu->fullscreenComponent, 1);
  } else {
    MenuListComponentSetCurrentString(&menu->fullscreenComponent, 0);
  }


  MenuVolumeComponentInit(&menu->musicComponent, board, textType, textTypeGray, "Music", menu->frame->_x + padding, menu->frame->_y + padding + (ySpaceBetweenComponents * 2));

  MenuVolumeComponentInit(&menu->effectComponent, board, textType, textTypeGray, "Sound FX", menu->frame->_x + padding, menu->frame->_y + padding + (ySpaceBetweenComponents * 3));

  MenuStringComponentInit(&menu->cancelComponent, board, textType, textTypeGray, "Cancel", menu->frame->_x + 70, menu->frame->_y + padding + (ySpaceBetweenComponents * 4) + 20);

  MenuStringComponentInit(&menu->applyComponent, board, textType, textTypeGray, "Apply", menu->frame->_x + 230, menu->frame->_y + padding + (ySpaceBetweenComponents * 4) + 20);

  menu->selectedComponent = COMPONENT_INDEX_RESOLUTION;
  MenuListComponentSetActive(&menu->resolutionComponent, true);

  menu->oldEffectValue = GLOBAL_PREFERENCES.effectVolume;
  menu->oldMusicValue = GLOBAL_PREFERENCES.musicVolume;
}

static void _setComponentActive(COIPreferencesMenu* menu, unsigned int componentIndex, bool active) {
  switch (componentIndex) {
  case COMPONENT_INDEX_RESOLUTION:
    MenuListComponentSetActive(&menu->resolutionComponent, active);
    break;
  case COMPONENT_INDEX_FULLSCREEN:
    MenuListComponentSetActive(&menu->fullscreenComponent, active);
    break;
  case COMPONENT_INDEX_MUSIC:
    MenuVolumeComponentSetActive(&menu->musicComponent, active);
    break;
  case COMPONENT_INDEX_EFFECT:
    MenuVolumeComponentSetActive(&menu->effectComponent, active);
    break;
  case COMPONENT_INDEX_CONFIRM:
    MenuStringComponentSetActive(&menu->cancelComponent, active);
    MenuStringComponentSetActive(&menu->applyComponent, false);
    break;
  default:
    printf("Invalid preferences menu component.\n");
  }
} 

static void _horizontalInputToComponent(COIPreferencesMenu* menu, unsigned int componentIndex, int val) {
  switch (componentIndex) {
  case COMPONENT_INDEX_RESOLUTION:
    MenuListComponentNext(&menu->resolutionComponent, val);
    menu->modifiedGraphics = true;
    break;
  case COMPONENT_INDEX_FULLSCREEN:
    MenuListComponentNext(&menu->fullscreenComponent, val);
    menu->modifiedGraphics = true;
    break;
  case COMPONENT_INDEX_MUSIC:
    if (!(GLOBAL_PREFERENCES.musicVolume == 0 && val < 0) && !(GLOBAL_PREFERENCES.musicVolume == COI_SOUND_MAX_VOLUME && val > 0)) {
      GLOBAL_PREFERENCES.musicVolume += (val * (COI_SOUND_MAX_VOLUME / VOLUME_BARS_COUNT));
      COISoundUpdateMusicVolume();
    }
    MenuVolumeComponentUpdateBarVisibility(&menu->musicComponent, GLOBAL_PREFERENCES.musicVolume);
    break;
  case COMPONENT_INDEX_EFFECT:
    if (!(GLOBAL_PREFERENCES.effectVolume == 0 && val < 0) && !(GLOBAL_PREFERENCES.effectVolume == COI_SOUND_MAX_VOLUME && val > 0)) {
      GLOBAL_PREFERENCES.effectVolume += (val * (COI_SOUND_MAX_VOLUME / VOLUME_BARS_COUNT));
      COISoundUpdateEffectVolume();
    }
    MenuVolumeComponentUpdateBarVisibility(&menu->effectComponent, GLOBAL_PREFERENCES.effectVolume);
    break;
  case COMPONENT_INDEX_CONFIRM:
    MenuStringComponentSetActive(&menu->cancelComponent, !menu->cancelComponent.label->visible);
    MenuStringComponentSetActive(&menu->applyComponent, !menu->applyComponent.label->visible);
    break;
  default:
    printf("Invalid preferences menu component.\n");
  }
}

void COIPreferencesMenuSetVisible(COIPreferencesMenu* menu, bool visible) {
  menu->frame->_visible = visible;
  MenuListComponentSetVisible(&menu->resolutionComponent, visible);
  MenuListComponentSetVisible(&menu->fullscreenComponent, visible);
  MenuVolumeComponentSetVisible(&menu->musicComponent, visible);
  MenuVolumeComponentSetVisible(&menu->effectComponent, visible);
  MenuStringComponentSetVisible(&menu->cancelComponent, visible);
  MenuStringComponentSetVisible(&menu->applyComponent, visible);

  if (visible) {
    menu->oldEffectValue = GLOBAL_PREFERENCES.effectVolume;
    menu->oldMusicValue = GLOBAL_PREFERENCES.musicVolume;
    menu->modifiedGraphics = false;
    MenuListComponentSetActive(&menu->resolutionComponent, true);
    MenuListComponentSetActive(&menu->fullscreenComponent, false);
    MenuVolumeComponentSetActive(&menu->musicComponent, false);
    MenuVolumeComponentSetActive(&menu->effectComponent, false);
    MenuVolumeComponentUpdateBarVisibility(&menu->musicComponent, GLOBAL_PREFERENCES.musicVolume);
    MenuVolumeComponentUpdateBarVisibility(&menu->effectComponent, GLOBAL_PREFERENCES.effectVolume);
    MenuStringComponentSetActive(&menu->cancelComponent, false);
    MenuStringComponentSetActive(&menu->applyComponent, false);
    menu->selectedComponent = COMPONENT_INDEX_RESOLUTION;
    for (int i = 0; i < RESOLUTION_COUNT; i++) {
      if (resolutions[i].width == GLOBAL_PREFERENCES.resolution.width && resolutions[i].height == GLOBAL_PREFERENCES.resolution.height) {
        MenuListComponentSetCurrentString(&menu->resolutionComponent, i);
        break;
      }
    }

    if (GLOBAL_PREFERENCES.fullscreen) {
      MenuListComponentSetCurrentString(&menu->fullscreenComponent, 1);
    } else {
      MenuListComponentSetCurrentString(&menu->fullscreenComponent, 0);
    }
  }
}

void COIPreferencesMenuProcessInput(COIPreferencesMenu* menu, int direction) {
  if (direction == MOVING_UP) {
    COISoundPlay(COI_SOUND_BLIP);
    unsigned int oldIndex = menu->selectedComponent;
    menu->selectedComponent = menu->selectedComponent == 0 ? COMPONENT_INDEX_CONFIRM : (menu->selectedComponent - 1);
    _setComponentActive(menu, oldIndex, false);
    _setComponentActive(menu, menu->selectedComponent, true);
  } else if (direction == MOVING_DOWN) {
    COISoundPlay(COI_SOUND_BLIP);
    unsigned int oldIndex = menu->selectedComponent;
    menu->selectedComponent = (menu->selectedComponent + 1) % 5;
    _setComponentActive(menu, oldIndex, false);
    _setComponentActive(menu, menu->selectedComponent, true);
  } else if (direction == MOVING_LEFT || direction == MOVING_RIGHT) {
    COISoundPlay(COI_SOUND_BLIP);
    _horizontalInputToComponent(menu, menu->selectedComponent, direction == MOVING_LEFT ? -1 : 1);
  } else if (direction == MOVING_SELECT && menu->selectedComponent == COMPONENT_INDEX_CONFIRM) {
    COISoundPlay(COI_SOUND_SELECT);
    if (MenuStringComponentIsActive(&menu->applyComponent)) {
      // Graphics settings
      GLOBAL_PREFERENCES.fullscreen = menu->fullscreenComponent.circleList.current->index == 1;
      GLOBAL_PREFERENCES.resolution = resolutions[menu->resolutionComponent.circleList.current->index];
      // Sound settings are already applied

      COIPreferencesWriteToFile();
      if (menu->modifiedGraphics) {
        COIWindowReloadPreferences(COI_GLOBAL_WINDOW);
      }
    } else {
      GLOBAL_PREFERENCES.musicVolume = menu->oldMusicValue;
      GLOBAL_PREFERENCES.effectVolume = menu->oldEffectValue;
      COISoundUpdateEffectVolume();
      COISoundUpdateMusicVolume();
    }
    COIPreferencesMenuSetVisible(menu, false);
  } else if (direction == MOVING_DELETE || direction == MOVING_PAUSE) {
    COISoundPlay(COI_SOUND_INVALID);
    GLOBAL_PREFERENCES.musicVolume = menu->oldMusicValue;
    GLOBAL_PREFERENCES.effectVolume = menu->oldEffectValue;
    COISoundUpdateEffectVolume();
    COISoundUpdateMusicVolume();

    COIPreferencesMenuSetVisible(menu, false);
  }
}

bool COIPreferenecesPrefDataExists() {
  char* filename = getPreferencesFileLocation();
  FILE* fp = fopen(filename, "r");
  if (fp == NULL) {
    return false;
  }
  fclose(fp);
  free(filename);
  return true;
}
