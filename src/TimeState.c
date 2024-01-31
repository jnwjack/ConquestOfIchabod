#include "TimeState.h"

TimeState GLOBAL_TIME = { TS_MORNING, 0 };

void TimeStateIncrement(unsigned char val) {
  GLOBAL_TIME.val += val;
  // Are we in a new time phase?
  if (GLOBAL_TIME.val >= TIMESTATE_PHASE_LENGTH) {
    GLOBAL_TIME.val = (GLOBAL_TIME.val - TIMESTATE_PHASE_LENGTH) % TIMESTATE_PHASE_LENGTH;
    GLOBAL_TIME.phase = (GLOBAL_TIME.phase + 1) % (TS_NIGHT + 1);
  }
}

  
