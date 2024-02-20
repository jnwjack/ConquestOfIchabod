#include "util.h"

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
  return random() & 0xFF;
}

bool generateRandomBoolWeighted(double weight) {
  char low = weight * 0xFF;
  return generateRandomChar() <= low;
}

// Randomly pick between left, right, up, or down
int generateRandomDirectionalMovement() {
  return (generateRandomChar() % 4) + 1;
}

