#include <SDL2/SDL_mixer.h>
#include "COISound.h"
#include "COIPreferences.h"

Mix_Chunk* blip = NULL;
Mix_Chunk* select_sound = NULL;
Mix_Chunk* celebration = NULL;
Mix_Chunk* invalid = NULL;
Mix_Chunk* hit = NULL;
Mix_Chunk* defeat = NULL;
Mix_Music* titleMusic = NULL;
Mix_Music* threadTownMusic = NULL;
Mix_Music* battleMusic = NULL;
Mix_Music* sludgeMusic = NULL;

Mix_Chunk* _loadSound(const char* soundString) {
  char temp[100];
  snprintf(temp, 100, "sound/%s", soundString);
  Mix_Chunk* chunk = Mix_LoadWAV(temp);
  if (!chunk) {
    printf("Error: Couldn't load sound %s\n", soundString);
  }

  return chunk;
}

Mix_Music* _loadMusic(const char* soundString) {
  char temp[100];
  snprintf(temp, 100, "sound/%s", soundString);
  Mix_Music* music = Mix_LoadMUS(temp);
  if (!music) {
    printf("Error: Couldn't load sound %s\n", soundString);
  }

  return music;  
}

void COISoundInit() {
  Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
  blip = _loadSound("cursor.wav");
  select_sound  = _loadSound("select.wav");
  celebration = _loadSound("celebration.wav");
  invalid = _loadSound("invalid.wav");
  hit = _loadSound("hit.wav");
  defeat = _loadSound("battle_death.wav");
  threadTownMusic = _loadMusic("threadtown.wav");
  titleMusic = _loadMusic("title.wav");
  battleMusic = _loadMusic("battle.wav");
  sludgeMusic = _loadMusic("sludge.wav");
}

void COISoundPlay(COISoundID sound) {
  switch (sound) {
  case COI_SOUND_BLIP:
    Mix_PlayChannel(-1, blip, 0);
    break;
  case COI_SOUND_SELECT:
    Mix_PlayChannel(-1, select_sound, 0);
    break;
  case COI_SOUND_CELEBRATION:
    Mix_HaltMusic();
    Mix_PlayChannel(-1, celebration, 0);
    break;
  case COI_SOUND_INVALID:
    Mix_PlayChannel(-1, invalid, 0);
    break;
  case COI_SOUND_HIT:
    Mix_PlayChannel(-1, hit, 0);
    break;
  case COI_SOUND_DEFEAT:
    Mix_HaltMusic();
    Mix_PlayChannel(-1, defeat, 0);
    break;
  case COI_SOUND_THREADTOWN:
    Mix_PlayMusic(threadTownMusic, 100);
    break;
  case COI_SOUND_TITLE:
    Mix_PlayMusic(titleMusic, 100);
    break;
  case COI_SOUND_BATTLE:
    Mix_PlayMusic(battleMusic, 100);
    break;
  case COI_SOUND_SLUDGE:
    Mix_PlayMusic(sludgeMusic, 100);
    break;
  default:
    printf("Error: Invalid sound ID.\n");
  }
}

void COISoundUpdateMusicVolume() {
  Mix_VolumeMusic(GLOBAL_PREFERENCES.musicVolume);
}
void COISoundUpdateEffectVolume() {
  Mix_MasterVolume(GLOBAL_PREFERENCES.effectVolume);  
}

void COISoundShutdown() {
  Mix_FreeChunk(blip);
  Mix_FreeChunk(select_sound);
  Mix_FreeChunk(celebration);
  Mix_FreeChunk(invalid);
  Mix_FreeChunk(defeat);
  Mix_FreeMusic(threadTownMusic);
  Mix_FreeMusic(titleMusic);
  Mix_FreeMusic(battleMusic);
  Mix_FreeMusic(sludgeMusic);
}