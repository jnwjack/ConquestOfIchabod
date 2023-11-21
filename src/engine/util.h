#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MIN(a,b) ((a > b) ? b : a)
#define MAX(a,b) ((a > b) ? a : b)

#define COI_PADDING 10

int countLines(const char* filename);
int generateRandomChar();
bool generateRandomBoolWeighted(double weight);

#endif
