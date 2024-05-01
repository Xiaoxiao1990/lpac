#include <stdio.h>
#include <string.h>

#include "times.h"

void get_datetime_string(struct date_time_str *t_str)
{
    struct timeval time;
    time_t now;
    char buf[20];// = "2018-04-08 14:09:00";

    gettimeofday(&time, NULL);
    now = time.tv_sec;
    strftime(buf, 20, "%Y-%m-%d %T", localtime(&now));
    snprintf(t_str->string, DATE_TIME_STRING_LEN, "%s.%6ld", buf, time.tv_usec);
}

void get_time_now(struct timeval *tv)
{
    gettimeofday(tv, NULL);
}

double time_use(struct timeval *start_time, struct timeval *end_time)//us
{
    return ((double)1000000*(end_time->tv_sec - start_time->tv_sec) + end_time->tv_usec - start_time->tv_usec);
}
