#include "util.h"
#include <assert.h>

#ifdef __WINDOWS__
/* This code is public domain -- Will Hartung 4/9/09 */
size_t getline(char **lineptr, size_t *n, FILE *stream) {
    char *bufptr = NULL;
    char *p = bufptr;
    size_t size;
    int c;

    if (lineptr == NULL) {
        return -1;
    }
    if (stream == NULL) {
        return -1;
    }
    if (n == NULL) {
        return -1;
    }
    bufptr = *lineptr;
    size = *n;

    c = fgetc(stream);
    if (c == EOF) {
        return -1;
    }
    if (bufptr == NULL) {
        bufptr = malloc(128);
        if (bufptr == NULL) {
            return -1;
        }
        size = 128;
    }
    p = bufptr;
    while(c != EOF) {
        if ((p - bufptr) > (size - 1)) {
            size = size + 128;
            bufptr = realloc(bufptr, size);
            if (bufptr == NULL) {
                return -1;
            }
        }
        *p++ = c;
        if (c == '\n') {
            break;
        }
        c = fgetc(stream);
    }

    *p++ = '\0';
    *lineptr = bufptr;
    *n = size;

    return p - bufptr - 1;
}
#endif

LinkedList* LinkedListCreate() {
  LinkedList* list = malloc(sizeof(LinkedList));
  list->head = NULL;
  list->tail = NULL;
  list->cursor = NULL;

  return list;
}

void LinkedListAdd(LinkedList* list, void* data) {
  if (list->tail == NULL) {
    // This is the first element
    list->tail = malloc(sizeof(LinkedListNode));
    list->tail->data = data;
    list->tail->next = NULL;
    list->head = list->tail;
    list->cursor = list->head;
  } else {
    LinkedListNode* newNode = malloc(sizeof(LinkedListNode));
    newNode->data = data;
    newNode->next = NULL;
    list->tail->next = newNode;
    list->tail = newNode;
  }
}

void LinkedListRemove(LinkedList* list, void* data) {
  LinkedListNode* current = list->head;
  if (current == NULL) {
    printf("Attempted to remove element from empty list.\n");
    return;
  } else if (current->data == data) {
    list->head = current->next;
    if (current == list->tail) {
      // If tail = head (1 element)
      list->tail = current->next;
    }
    free(current);
  } else {
    LinkedListNode* previous = current;
    current = current->next;
    while (current != NULL) {
      if (current->data == data) {
	if (current == list->tail) {
	  // If it's the last element, then tail needs to be adjusted.
	  list->tail = previous;
	}
	previous->next = current->next;
	free(current);
	return;
      }
      previous = current;
      current = current->next;
    }

    printf("Attempted to remove element from list that is not present in list.\n");
  }
}

void* LinkedListNext(LinkedList* list) {
  if (list->cursor == NULL) {
    return NULL;
  }
  
  void* d = list->cursor->data;
  list->cursor = list->cursor->next;
  return d;
}

void LinkedListResetCursor(LinkedList* list) {
  list->cursor = list->head;
}

void LinkedListDestroy(LinkedList* list) {
  LinkedListNode* current = list->head;
  while (current != NULL) {
    LinkedListNode* temp = current->next;
    free(current);
    current = temp;
  }

  free(list);
}

void IntListInitialize(IntList* list, int capacity) {
  list->capacity = capacity;
  list->values = malloc(sizeof(int) * capacity);
  list->length = 0;
}

void IntListAdd(IntList* list, int value) {
  if (list->length < list->capacity) {
    list->values[list->length] = value;
    list->length++;
  } else {
    printf("Error: attempted to add to full list.");
  }
}

void IntListDelete(IntList* list, int index) {
  assert(index >= 0 && index < list->length);

  // Starting at index + 1, shift every element one place
  // to the left.
  for (int i = index + 1; i < list->length; i++) {
    list->values[i - 1] = list->values[i];
  }
  list->length--;
}

void IntListDestroy(IntList* list) {
  free(list->values);
}

int POW_INT(int x, int y) {
  int res = 1;
  for (int i = 0; i < y; i++) {
    res *= x;
  }

  return res;
}

int countLines(const char* filename) {
  FILE* fp = NULL;
  char c;
  int n = 0;

  fp = fopen(filename, "r");
  while (!feof(fp)) {
    c = fgetc(fp);
    if (c == '\n') {
      n++;
    }
  }

  return n;
}

int generateRandomChar() {
  return rand() & 0xFF;
}

bool generateRandomBoolWeighted(double weight) {
  int low = weight * 0xFF;
  int randomVal = generateRandomChar();
  return randomVal <= low;
}

int generateRandomCharInRange(int lowInclusive, int highInclusive) {
  int diff = (highInclusive + 1) - lowInclusive;
  return (generateRandomChar() % diff) + lowInclusive;
}

// Randomly pick between left, right, up, or down
int generateRandomDirectionalMovement() {
  return (generateRandomChar() % 4) + 1;
}

int generateRandomCharWithCenter(int center, int rangeSize) {
  return generateRandomCharInRange(center - rangeSize, center + rangeSize);
}

bool inputIsDirection(int input) {
  return input == MOVING_UP || input == MOVING_DOWN || input == MOVING_LEFT || input == MOVING_RIGHT || input == MOVING_NONE;
}

void scaleRect(float factorW, float factorH, SDL_Rect* src, SDL_Rect* dst) {
  if (src) {
    dst->h = src->h * factorH;
    dst->w = src->w * factorW;
    dst->y = src->y * factorH;
    dst->x = src->x * factorW;
  }
}

char* getDataDir() {
  return SDL_GetPrefPath(DATA_DIR_ORG, DATA_DIR_PROGRAM);
}

char* getSaveFileLocation() {
  char* dataDir = getDataDir();
  char* filename = malloc(sizeof(char) * 200);
  snprintf(filename, 200, "%s%s", dataDir, "save.dat");
  SDL_free(dataDir);
  return filename;
}

char* getPreferencesFileLocation() {
  char* dataDir = getDataDir();
  char* filename = malloc(sizeof(char) * 200);
  snprintf(filename, 200, "%s%s", dataDir, "preferences.dat");
  SDL_free(dataDir);
  return filename;
}

