#ifndef COICHAR_H
#define COICHAR_H

typedef struct COIChar {
  SDL_Rect* _drawRect;
  SDL_Texture* _texture;
  
  int x;
  int y;
  bool visible;

  char value;
}COIChar;

COIChar* COICharCreate(char c, COITextType* textType)
void COICharDestroy(COIChar* coiChar);



#endif
