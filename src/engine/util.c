#include "util.h"

int countLines(const char* filename) {
  FILE* fp;
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
