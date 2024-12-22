#ifndef COI_SOUND_H
#define COI_SOUND_H

#define COI_SOUND_MAX_VOLUME 128

typedef enum COISoundID {
  COI_SOUND_CELEBRATION,
  COI_SOUND_BLIP,
  COI_SOUND_SELECT,
  COI_SOUND_INVALID,
  COI_SOUND_HIT,
  COI_SOUND_DEFEAT,
  COI_SOUND_THREADTOWN,
  COI_SOUND_TITLE,
  COI_SOUND_BATTLE,
  COI_SOUND_SLUDGE
} COISoundID;

void COISoundPlay(COISoundID sound);
void COISoundUpdateMusicVolume();
void COISoundUpdateEffectVolume();
void COISoundInit();

#endif