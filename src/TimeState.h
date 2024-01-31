#ifndef TIMESTATE_H
#define TIMESTATE_H

/*
#define TIMESTATE_MORNING_THRESHOLD 5
#define TIMESTATE_DAY_THRESHOLD 10
#define TIMESTATE_EVENING_THRESHOLD 18
#define TIMESTATE_NIGHT_THRESHOLD 22
*/

#define TIMESTATE_PHASE_LENGTH 2

typedef enum {
  TS_MORNING = 0,
  TS_DAY,
  TS_EVENING,
  TS_NIGHT
} TimeStatePhase;


typedef struct TimeState {
  TimeStatePhase phase;
  unsigned char val;
} TimeState;

void TimeStateIncrement(unsigned char val);

extern TimeState GLOBAL_TIME;

#endif
