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
#define SA_DAYS_OLDEST 65

#define CLERK_CLASS_CHANGE_SHIFTS_WORKED 15

#define GRASS_PATH_SHIFTS_WORKED 10
#define CORRUPTION_1_DAYS 22
#define CORRUPTION_2_DAYS 43
#define CORRUPTION_3_DAYS 68
#define CORRUPTION_4_DAYS 85

#define BATTLE_SET_1_DAYS 35
#define BATTLE_SET_2_DAYS 65
#define BATTLE_GREEN_GRASS_EXPANDED_ENEMIES_DAYS 5

#define BOARD_COLOR_CHANGE_1_DAYS 31
#define BOARD_COLOR_CHANGE_2_DAYS 53
#define BOARD_COLOR_CHANGE_3_DAYS 76

#define RENTHOUSE_BALD_DAYS 32

#define TREE_GUY_CONVO_1_DAYS 18

#define FINAL_DAY 100

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
