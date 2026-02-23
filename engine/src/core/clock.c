#include "clock.h"
#include "platform/platform.h"
#include "logger.h"
#include "kassert.h"

void clock_start(clock* clock)
{
    KASSERT_MSG(clock, "clock is null");
    clock->start_time = platform_get_absolute_time();
    clock->elapsed_time = 0;
}

void clock_stop(clock* clock)
{
    KASSERT_MSG(clock, "clock is null");
    clock->start_time = 0;
}

void clock_update(clock* clock)
{
    KASSERT_MSG(clock, "clock is null");
    if(clock->start_time!=0.0)
    {
        clock->elapsed_time = platform_get_absolute_time() - clock->start_time;
    }
}