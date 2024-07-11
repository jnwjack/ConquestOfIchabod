#ifndef COI_SOUND_H
#define COI_SOUND_H

typedef enum COISoundID {
  COI_SOUND_CELEBRATION,
  COI_SOUND_BLIP,
  COI_SOUND_SELECT,
  COI_SOUND_INVALID,
  COI_SOUND_HIT
} COISoundID;

void COISoundPlay(COISoundID sound);
void COISoundInit();

#endif