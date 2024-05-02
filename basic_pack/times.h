#ifndef __TIME_H__
#define __TIME_H__

#include <time.h>
#include <sys/time.h>

#ifdef __cplusplus
extern "C"{
#endif

#define DATE_TIME_STRING_LEN    40

struct date_time_str{
    char string[DATE_TIME_STRING_LEN];
};

void get_datetime_string(struct date_time_str *t_str);
void get_time_now(struct timeval *tv);
double time_use(struct timeval *start_time, struct timeval *end_time);//us

#ifdef __cplusplus
}
#endif
#endif /* __TIME_H__ */
