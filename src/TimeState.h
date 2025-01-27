#ifndef TIMESTATE_H
#define TIMESTATE_H

#include <stdbool.h>

/*
#define TIMESTATE_MORNING_THRESHOLD 5
#define TIMESTATE_DAY_THRESHOLD 10
#define TIMESTATE_EVENING_THRESHOLD 18
#define TIMESTATE_NIGHT_THRESHOLD 22
*/

#define TIMESTATE_PHASE_LENGTH 6

// Time related constants
#define SA_DAYS_OLDER 30
#define SA_DAYS_OLDEST 60

#define CLERK_CLASS_CHANGE_SHIFTS_WORKED 15

#define GRASS_PATH_SHIFTS_WORKED 10
#define CORRUPTION_1_DAYS 15
#define CORRUPTION_2_DAYS 27
#define CORRUPTION_3_DAYS 45
#define CORRUPTION_4_DAYS 68

#define BATTLE_SET_1_DAYS 17
#define BATTLE_SET_2_DAYS 40
#define BATTLE_GREEN_GRASS_EXPANDED_ENEMIES_DAYS 5

#define BOARD_COLOR_CHANGE_1_DAYS 25
#define BOARD_COLOR_CHANGE_2_DAYS 41
#define BOARD_COLOR_CHANGE_3_DAYS 63

#define RENTHOUSE_BALD_DAYS 8

#define TREE_GUY_CONVO_1_DAYS 10
#define TREE_GUY_CONVO_2_DAYS 24
#define TREE_GUY_CONVO_3_DAYS 43

#define PAUSE_OVERLAY_GOLD_DAYS 30
#define OMINOUS_MESSAGE_DAYS 3
#define MOCKING_MESSAGE_DAYS 14
#define VISITING_MESSAGE_DAYS 6

#define TIRED_MESSAGE_DAYS 5
#define WELCOME_HOME_DAYS 75
#define FELT_DIFFERENT_DAYS 12
#define GOTTEN_FATTER_DAYS 36
#define LEAVING_THIS_PLACE_DAYS 47
#define LAST_XP_GAIN_DAYS 5
#define BREAK_FROM_WORKING_SHIFTS_WORKED 4

#define HAVONVALE_TEXT_1_DAYS 15
#define HAVONVALE_TEXT_2_DAYS 26

#define ENCOURAGEMENT_TEXT_1_DAYS 36
#define ENCOURAGEMENT_TEXT_2_DAYS 13

#define CREEPY_SPRITEMAP_DAYS 8

#define NOSTALGIC_TEXT_1_DAYS 30

#define TIME_GUY_TEXT_1_DAYS 56

#define FINAL_DAY 80

typedef enum {
  TS_MORNING = 0,
  TS_DAY = 1,
  TS_EVENING = 2,
  TS_NIGHT = 3
} TimeStatePhase;

typedef struct TimeState {
  TimeStatePhase phase;
  unsigned char val;
  unsigned long day;
} TimeState;

void TimeStateAddVal(TimeState* state, unsigned char val);
void TimeStateIncrement(unsigned char val);
bool TimeStateInFuture(TimeState* state);
void TimeStateCopyGlobalTime(TimeState* state);

extern TimeState GLOBAL_TIME;
extern TimeState END_TIME; // Causes game over when reached

#endif
