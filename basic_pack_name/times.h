#ifndef __TIME_H__
#define __TIME_H__

#include "types.h"
#include <time.h>
#include <sys/time.h>

#ifdef __cplusplus
extern "C"{
#endif

#define DATE_TIME_STRING_LEN    40
#define TIME2STR_LEN            40

struct date_time_str{
    char string[DATE_TIME_STRING_LEN];
};

typedef struct {
    u16 year;
    u8 month;
    u8 day;
    u8 hour;
    u8 minute;
    u8 second;
    u32 u_second;
    u8 week;
} datetime_t;

void get_datetime_string(struct date_time_str *t_str);
const char *get_datetime_string_fmt(struct date_time_str *t_str, const char *fmt);
void get_time_now(struct timeval *tv);
double time_use(struct timeval *start_time, struct timeval *end_time);//us
void get_local_time(datetime_t *date_time);
int get_current_time_hour(void);
int get_current_time_minute(void);
int get_current_time_second(void);
int set_timezone(int tz);
int set_system_time(long long timestamp);
void get_time_string(char *buff, const char *time_fmt);
const char *get_time_stamp_string(void);

// format "%Y-%m-%d %H:%M:%S"
const char *time_stamp_to_date(u64 timestamp, const char *time_fmt);
const char *time_stamp_to_date_r(u64 timestamp, const char *time_fmt, struct date_time_str *dts);
long long get_time_stamp_value(void);

#ifdef __cplusplus
}
#endif
#endif /* __TIME_H__ */
