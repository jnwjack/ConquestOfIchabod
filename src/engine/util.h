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


int countLines(const char* filename);
int generateRandomChar();
bool generateRandomBoolWeighted(double weight);

#endif
