#include "_time.h"

double get_timestamp()
{
    return clock();
}

double get_time_interval(double timestamp1, double timestamp2)
{
    return fabs((double)(timestamp2 - timestamp1) / CLOCKS_PER_SEC);
}

int round_time(double time)
{
    return (int)(time);
}