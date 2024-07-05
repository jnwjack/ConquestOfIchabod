#include <SDL2/SDL_mixer.h>
#include "COISound.h"

Mix_Chunk* blip = NULL;
Mix_Chunk* select = NULL;

Mix_Chunk* _loadSound(const char* soundString) {
  char temp[100];
  snprintf(temp, 100, "sound/%s", soundString);
  Mix_Chunk* chunk = Mix_LoadWAV(temp);
  if (!chunk) {
    printf("Error: Couldn't load sound %s\n", soundString);
  }

  return chunk;
}

void COISoundInit() {
  Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
  blip = _loadSound("cursor.wav");
  select  = _loadSound("select.wav");
}

void COISoundPlay(COISoundID sound) {
  switch (sound) {
  case COI_SOUND_BLIP:
    Mix_PlayChannel( -1, blip, 0 );
    break;
  case COI_SOUND_SELECT:
    Mix_PlayChannel( -1, select, 0 );
    break;
  default:
    printf("Error: Invalid sound ID.\n");
  }
}