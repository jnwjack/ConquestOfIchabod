#include "COITextGroup.h"

COITextGroup* COITextGroupCreate(int fontSize, int r, int g, int b, const char* filename, SDL_Renderer* renderer) {
  COITextGroup* group = malloc(sizeof(COITextGroup));
  group->_font = TTF_OpenFont("src/engine/etc/font.ttf", fontSize);
  group->_color = { r, g, b };

  FILE* fp;

  fp = fopen(filename, "r");
  if (fp == NULL) {
    return;
  }

  group->_textCount = countLines(filename);
  group->_texts = malloc(group->_textCount * sizeof(COIText*));

  size_t len = 0;
  char* line, textString;
  int i;
  SDL_Surface* surface;
  COIText* text;
  while (getline(&line, &len, fp) != -1) {
    line[strcspn(line, "\n")] = '\0';
    
    text = malloc(sizeof(COIText));
    
    textString = strtok(line, " ");
    text->_x = atoi(strtok(NULL, " "));
    text->_y = atoi(strtok(NULL, " "));
    text->_width = 0;
    text->_height = 0;
    
    text->_string = malloc(strlen(textString));
    strcpy(text->_string, textString);
    surface = TTF_RenderText_Solid(group->_font, text->_string, group->_color);
    text->_texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_QueryTexture(text->_texture, NULL, NULL, &text->_width, &text->_height);
    text->_drawRect = { text->_x, text->_y, text->width, text->_height };
    text->_visible = false;
    group->_texts[i] = text;
        
    SDL_FreeSurface(surface);
        
    i++;
  }
}

void COITextGroupDestroy(COITextGroup* group) {
  if (group == NULL) {
    return;
  }
  
  int i;
  COIText* text;
  for (i = 0; i < group->_textCount; i++) {
    text = group->_texts[i];
    if (text == NULL) {
      continue;
    }

    free(text->_string);
    SDL_DestroyTexture(text->_texture);
    free(text);
  }

  TTF_CloseFont(group->_font);
}
