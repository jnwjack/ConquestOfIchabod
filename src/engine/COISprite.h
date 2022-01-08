#ifndef COISPRITE_H
#define COISPRITE_H

typedef struct COISprite {
  int _x;
  int _y;
}COISprite;

COISprite* COISpriteCreate();
void COISpriteDestroy(COISprite* sprite);

#endif
