#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

#define MIN(a,b) ((a > b) ? b : a)
#define MAX(a,b) ((a > b) ? a : b)

int countLines(const char* filename);

#endif
