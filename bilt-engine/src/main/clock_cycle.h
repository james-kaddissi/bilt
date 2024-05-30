#pragma once

#include "defines.h"

typedef struct clock {
    f64 begin_time;
    f64 running_duration;
} clock;

void update_clock(clock* clock);
void start_clock(clock* clock);
void stop_clock(clock* clock);