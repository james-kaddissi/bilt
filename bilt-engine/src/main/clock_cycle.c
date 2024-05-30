#include "main/clock_cycle.h"
#include "platforms/unify_platforms.h"

void update_clock(clock* clock) {
    if(clock->begin_time != 0) {
        clock->running_duration = get_time() - clock->begin_time;
    }
}

void start_clock(clock* clock) {
    clock -> begin_time = get_time();
    clock -> running_duration = 0;
}

void stop_clock(clock* clock) {
    clock -> begin_time = 0;
}