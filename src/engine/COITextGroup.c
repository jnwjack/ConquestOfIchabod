#include "COITextGroup.h"

COITextGroup* COITextGroupCreate(int fontSize, int r, int g, int b, const char* filename, SDL_Renderer* renderer) {
  COITextGroup* group = malloc(sizeof(COITextGroup));
  group->_font = TTF_OpenFont("src/engine/etc/font.ttf", fontSize);
  if (group->_font == NULL) {
    printf("Error loading font");
  }
  group->_color.r = r;
  group->_color.g = g;
  group->_color.b = b;
  group->_fontSize = fontSize;


  FILE* fp = NULL;

  fp = fopen(filename, "r");
  if (fp == NULL) {
    return NULL;
  }

  group->_textCount = countLines(filename);
  group->_texts = malloc(group->_textCount * sizeof(COIText*));

  size_t len = 0;
  char* line = NULL;
  char* textString = NULL;
  int i = 0;
  SDL_Surface* surface = NULL;
  COIText* text = NULL;
  while (getline(&line, &len, fp) != -1) {
    line[strcspn(line, "\n")] = '\0';
    
    text = malloc(sizeof(COIText));
    
    textString = strtok(line, " ");
    text->_x = 0;
    text->_y = 0;
    text->_width = 0;
    text->_height = 0;

    text->_string = malloc(strlen(textString));
    strcpy(text->_string, textString);
    surface = TTF_RenderText_Solid(group->_font, text->_string, group->_color);
    text->_texture = SDL_CreateTextureFromSurface(renderer, surface);
    int w, h;
    SDL_QueryTexture(text->_texture, NULL, NULL, &w, &h);
    text->_drawRect = malloc(sizeof(SDL_Rect));
    text->_drawRect->x = text->_x;
    text->_drawRect->y = text->_y;
    text->_drawRect->w = w;
    text->_drawRect->h = h;
    text->_width = w;
    text->_height = h;
    text->_visible = false;
    group->_texts[i] = text;
        
    SDL_FreeSurface(surface);
        
    i++;
  }

  return group;
}

void COITextGroupDestroy(COITextGroup* group) {
  if (group == NULL) {
    return;
  }
  
  int i;
  COIText* text = NULL;
  for (i = 0; i < group->_textCount; i++) {
    text = group->_texts[i];
    if (text == NULL) {
      continue;
    }

    free(text->_string);
    free(text->_drawRect);
    SDL_DestroyTexture(text->_texture);
    free(text);
  }

  TTF_CloseFont(group->_font);
}

COIText** COITextGroupGetTexts(COITextGroup* group) {
  return group->_texts;
}

int COITextGroupGetTextCount(COITextGroup* group) {
  return group->_textCount;
}

void COITextSetPos(COIText* text, int x, int y) {
  text->_x = x;
  text->_y = y;

  text->_drawRect->x = x;
  text->_drawRect->y = y;
}
