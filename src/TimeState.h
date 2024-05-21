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
