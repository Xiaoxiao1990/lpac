#include "times.h"
#include "logs.h"

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

// %Y-%m-%d_%T
const char* get_datetime_string_fmt(struct date_time_str* t_str, const char* fmt)
{
    struct timeval time;
    time_t now;

    gettimeofday(&time, NULL);
    now = time.tv_sec;
    strftime(t_str->string, DATE_TIME_STRING_LEN, fmt, localtime(&now));

    return t_str->string;
}

void get_datetime_string(struct date_time_str* t_str)
{
    struct timeval time;
    time_t now;
    char buf[20];// = "2018-04-08 14:09:00";

    gettimeofday(&time, NULL);
    now = time.tv_sec;
    strftime(buf, 20, "%Y-%m-%d %T", localtime(&now));
    snprintf(t_str->string, DATE_TIME_STRING_LEN, "%s.%06ld", buf, time.tv_usec);
}

void get_time_now(struct timeval* tv)
{
    gettimeofday(tv, NULL);
}

double time_use(struct timeval* start_time, struct timeval* end_time)//us
{
    return ((double)1000000 * (end_time->tv_sec - start_time->tv_sec) + end_time->tv_usec - start_time->tv_usec);
}

void get_local_time(datetime_t* date_time)
{
    struct timeval tv;
    struct tm w;
    gettimeofday(&tv, NULL);
    localtime_r(&tv.tv_sec, &w);

    date_time->year = w.tm_year + 1900;
    date_time->month = w.tm_mon + 1;
    date_time->day = w.tm_mday;
    date_time->hour = w.tm_hour;
    date_time->minute = w.tm_min;
    date_time->second = w.tm_sec;
    date_time->u_second = tv.tv_usec;
    date_time->week = 0;
}

static char time_stamp_string[TIME2STR_LEN];

const char* get_time_stamp_string(void)
{
    struct timeval us;

    gettimeofday(&us, NULL);
    memset(time_stamp_string, 0, TIME2STR_LEN);
    snprintf(time_stamp_string, TIME2STR_LEN, "%lld", (long long)us.tv_sec * 1000 + us.tv_usec / 1000);
    return time_stamp_string;
}

// format "%Y-%m-%d %H:%M:%S"
const char* time_stamp_to_date(u64 timestamp, const char* time_fmt)
{
    time_t now = timestamp / 1000;

    memset(time_stamp_string, 0, TIME2STR_LEN);
    strftime(time_stamp_string, TIME2STR_LEN, time_fmt, localtime(&now));

    return time_stamp_string;
}

// format "%Y-%m-%d %H:%M:%S"
const char* time_stamp_to_date_r(u64 timestamp, const char* time_fmt, struct date_time_str* dts)
{
    time_t now = timestamp / 1000;

    memset(dts->string, 0, DATE_TIME_STRING_LEN);
    strftime(dts->string, DATE_TIME_STRING_LEN, time_fmt, localtime(&now));

    return dts->string;
}

long long get_time_stamp_value(void)
{
    struct timeval us;

    gettimeofday(&us, NULL);
    return (long long)us.tv_sec * 1000 + us.tv_usec / 1000;
}

int set_timezone(int tz)
{
    char tzstr[256] = { 0 };
    int tzhour = -tz;

    snprintf(tzstr, 255, "GMT%+02d", tzhour);
    if (setenv("TZ", tzstr, 1) != 0) {
        LOGE("setenv failed!");
        return -1;
    }

    LOGI("time zone: %s!", tzstr);
    tzset();

    return 0;
}
/************************************************
设置操作系统时间
参数: timestamp in second
调用方法:
    timestamp = 1625049927 //
    set_system_time(timestamp); 2021-06-30 18:45:27 (Beijing)
**************************************************/
int set_system_time(long long timestamp)
{
    struct tm tm;
    struct tm _tm;
    struct timeval tv;
    time_t timep;
    //    const char *time_fmt = "%Y-%m-%d %H:%M:%S";

    struct date_time_str start_time;

    time_stamp_to_date_r(timestamp, "%Y-%m-%d %T", &start_time);

    //    LOGI("current time: %s", time_stamp_to_date(get_time_stamp_value() / 1000, time_fmt));
    //    LOGI("target time: %s", time_stamp_to_date(timestamp, time_fmt));
    //    set_timezone(0); // reset time zone
    sscanf(start_time.string, "%d-%d-%d %d:%d:%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday, &tm.tm_hour, &tm.tm_min, &tm.tm_sec);

    _tm.tm_sec = tm.tm_sec;
    _tm.tm_min = tm.tm_min;
    _tm.tm_hour = tm.tm_hour;
    _tm.tm_mday = tm.tm_mday;
    _tm.tm_mon = tm.tm_mon - 1;
    _tm.tm_year = tm.tm_year - 1900;

    timep = mktime(&_tm);
    tv.tv_sec = timep;
    tv.tv_usec = (timep % 1000) * 1000;

    //    LOGW("S: %ld, s: %ld", tv.tv_sec, tv.tv_usec);

    if (settimeofday(&tv, NULL) < 0) {
        LOGE("Set system date time error: %s!", strerror(errno));
        return -1;
    }

    //    set_timezone(8);

    //    execute_shell_command("hwclock -w");

    return 0;
}

int get_current_time_hour(void)
{
    time_t seconds;
    struct tm now;

    seconds = time(0);
    localtime_r(&seconds, &now);

    //	printf("%d-%d-%d ", (1900 + now.tm_year), (1 + now.tm_mon), now.tm_mday);
    //	printf("%d:%d:%d\n", now.tm_hour, now.tm_min, now.tm_sec);

    return now.tm_hour;
}

int get_current_time_minute(void)
{
    time_t seconds;
    struct tm now;

    seconds = time(0);
    localtime_r(&seconds, &now);

    //	printf("%d-%d-%d ", (1900 + now.tm_year), (1 + now.tm_mon), now.tm_mday);
    //	printf("%d:%d:%d\n", now.tm_hour, now.tm_min, now.tm_sec);

    return now.tm_min;
}

int get_current_time_second(void)
{
    time_t seconds;
    struct tm now;

    seconds = time(0);
    localtime_r(&seconds, &now);

    //	printf("%d-%d-%d ", (1900 + now.tm_year), (1 + now.tm_mon), now.tm_mday);
    //	printf("%d:%d:%d\n", now.tm_hour, now.tm_min, now.tm_sec);

    return now.tm_sec;
}

void get_time_string(char* buff, const char* time_fmt)
{
    struct timeval time;
    time_t now;

    gettimeofday(&time, NULL);
    now = time.tv_sec;

    strftime(buff, TIME2STR_LEN, time_fmt, localtime(&now));
}