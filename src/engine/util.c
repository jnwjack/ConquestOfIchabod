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
    free(current);
  } else {
    LinkedListNode* previous = current;
    current = current->next;
    while (current != NULL) {
      if (current->data == data) {
	previous->next = current->next;
	free(current);
	return;
      }
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

