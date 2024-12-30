#include "TimeState.h"
#include <stdio.h>

TimeState GLOBAL_TIME = { TS_MORNING, 0, 0 };
TimeState END_TIME = { TS_MORNING, 0, FINAL_DAY };

void TimeStateAddVal(TimeState* state, unsigned char val) {
  state->val += val;
  // Are we in a new time phase?
  if (state->val >= TIMESTATE_PHASE_LENGTH) {
    TimeStatePhase oldPhase = state->phase;
    state->phase += state->val / (TIMESTATE_PHASE_LENGTH);
    state->val = (state->val - TIMESTATE_PHASE_LENGTH) % TIMESTATE_PHASE_LENGTH;
    state->phase = (state->phase) % (TS_NIGHT + 1);

    // If our new phase <= our old phase, we're in a new day
    if (state->phase <= oldPhase) {
      if (state->day > 150) {
        state->day += 2; // After day 150, time moves quicker.
      } else {
        state->day++;
      }
    }
  }
}

void TimeStateIncrement(unsigned char val) {
  TimeStateAddVal(&GLOBAL_TIME, val);
}

bool TimeStateInFuture(TimeState* state) {
  if (state->day != GLOBAL_TIME.day) {
    return state->day > GLOBAL_TIME.day;
  } else if (state->phase != GLOBAL_TIME.phase) {
    return state->phase > GLOBAL_TIME.phase;
  }
  return state->val > GLOBAL_TIME.val;
}

void TimeStateCopyGlobalTime(TimeState* state) {
  state->day = GLOBAL_TIME.day;
  state->phase = GLOBAL_TIME.phase;
  state->val = GLOBAL_TIME.val;
}

  
