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
Mix_Music* fairyMusic = NULL;
Mix_Music* sludgeNormalMusic = NULL;
Mix_Music* gemstonesMusic = NULL;

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
    printf("Error: Couldn't load sound %s\n", temp);
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
  threadTownMusic = _loadMusic("threadtown.mp3");
  titleMusic = _loadMusic("title.mp3");
  battleMusic = _loadMusic("battle.mp3");
  sludgeMusic = _loadMusic("sludge.mp3");
  fairyMusic = _loadMusic("fairy.mp3");
  sludgeNormalMusic = _loadMusic("sludge_normal.mp3");
  gemstonesMusic = _loadMusic("gemstones.mp3");
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
    Mix_PlayMusic(threadTownMusic, -1);
    break;
  case COI_SOUND_TITLE:
    Mix_PlayMusic(titleMusic, -1);
    break;
  case COI_SOUND_BATTLE:
    Mix_PlayMusic(battleMusic, -1);
    break;
  case COI_SOUND_SLUDGE:
    Mix_PlayMusic(sludgeMusic, -1);
    break;
  case COI_SOUND_FAIRY:
    Mix_PlayMusic(fairyMusic, -1);
    break;
  case COI_SOUND_SLUDGE_NORMAL:
    Mix_PlayMusic(sludgeNormalMusic, -1);
    break;
  case COI_SOUND_GEMSTONES:
    Mix_PlayMusic(gemstonesMusic, -1);
  default:
    printf("Error: Invalid sound ID.\n");
  }
}

void COISoundUpdateMusicVolume() {
  Mix_VolumeMusic(GLOBAL_PREFERENCES.musicVolume);
}
void COISoundUpdateEffectVolume() {
  // Mix_MasterVolume(GLOBAL_PREFERENCES.effectVolume);
  Mix_Volume(-1, GLOBAL_PREFERENCES.effectVolume);
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
  Mix_FreeMusic(fairyMusic);
  Mix_FreeMusic(sludgeNormalMusic);
  Mix_FreeMusic(gemstonesMusic);
}