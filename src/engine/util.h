#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MIN(a,b) ((a > b) ? b : a)
#define MAX(a,b) ((a > b) ? a : b)
#define ABS_DIFF(a,b) ((a > b) ? a - b : b - a)

#define COI_PADDING 10
#define COIBOARD_GRID_SIZE 32

#define MOVING_NONE 0
#define MOVING_LEFT 1
#define MOVING_RIGHT 2
#define MOVING_UP 3
#define MOVING_DOWN 4
#define MOVING_SELECT 5
#define MOVING_DELETE 6

struct LinkedListNode;
typedef struct LinkedListNode {
  void* data;
  struct LinkedListNode* next;
} LinkedListNode;

typedef struct LinkedList {
  LinkedListNode* head;
  LinkedListNode* tail; // Add is O(1)
  LinkedListNode* cursor; // Used when traversing list
} LinkedList;


LinkedList* LinkedListCreate();
void LinkedListAdd(LinkedList* list, void* data);
void LinkedListRemove(LinkedList* list, void* data);
void* LinkedListNext(LinkedList* list);
void LinkedListResetCursor(LinkedList* list);
void LinkedListDestroy(LinkedList* list);

// "Add and Read" list. Attempting to delete an element from the underlying array
// will make bad things happen.
// Can make this more generic later on.
typedef struct IntList {
  int* values;
  int capacity;
  int length;
} IntList;

void IntListInitialize(IntList* list, int capacity);
void IntListDestroy(IntList* list);
void IntListAdd(IntList* list, int value);

int POW_INT(int x, int y);

int countLines(const char* filename);
int generateRandomChar();
bool generateRandomBoolWeighted(double weight);
int generateRandomCharInRange(int lowInclusive, int highInclusive);
int generateRandomDirectionalMovement();

#endif
