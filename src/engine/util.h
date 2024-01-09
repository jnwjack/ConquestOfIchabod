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

//TODO: Linked List

int countLines(const char* filename);
int generateRandomChar();
bool generateRandomBoolWeighted(double weight);

#endif
