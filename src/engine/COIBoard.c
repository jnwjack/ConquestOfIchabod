#include "COIBoard.h"

COIBoard* COIBoardCreate(int r, int g, int b, int a) {
  COIBoard* board = malloc(sizeof(COIBoard));
  board->_bgColor[INDEX_RED] = r;
  board->_bgColor[INDEX_GREEN] = g;
  board->_bgColor[INDEX_BLUE] = b;
  board->_bgColor[INDEX_ALPHA] = a;
  board->_sprites = NULL;
  return board;
}

void COIBoardDestroy(COIBoard* board) {
  if (board->_sprites != NULL) {
    free(board->_sprites);
  }
  free(board);
}

int COIBoardBGColor(COIBoard* board, int index) {
  return board->_bgColor[index];
}
