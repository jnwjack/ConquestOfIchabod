#include "util.h"

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

