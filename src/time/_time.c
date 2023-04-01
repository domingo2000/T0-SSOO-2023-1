#include "_time.h"

double get_timestamp()
{
    return clock() / CLOCKS_PER_SEC;
}

double get_time_interval(double timestamp1, double timestamp2)
{
    return fabs(timestamp2 - timestamp1);
}

int round_time(double time)
{
    return (int)round((time));
}