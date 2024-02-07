#include "TimeState.h"

TimeState GLOBAL_TIME = { TS_MORNING, 0, 0 };

void TimeStateIncrement(unsigned char val) {
  GLOBAL_TIME.val += val;
  // Are we in a new time phase?
  if (GLOBAL_TIME.val >= TIMESTATE_PHASE_LENGTH) {
    TimeStatePhase oldPhase = GLOBAL_TIME.phase;
    GLOBAL_TIME.val = (GLOBAL_TIME.val - TIMESTATE_PHASE_LENGTH) % TIMESTATE_PHASE_LENGTH;
    GLOBAL_TIME.phase = (GLOBAL_TIME.phase + 1) % (TS_NIGHT + 1);

    // If our new phase <= our old phase, we're in a new day
    if (GLOBAL_TIME.phase <= oldPhase) {
      GLOBAL_TIME.day++;
    }
  }
}

  
