/*********************************************************************
 * Author: Zhou Linlin
 * E-mail: 461146760@qq.com
 * Date  : 2022-06-10 10:08:21.
 * Copyright (c) 2022 all rights reserved.
*********************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "logs.h"
#include "times.h"
#include "utils.h"


static bool log_is_init = false;
static int log_output = 1;

int get_log_output(void)
{
    return log_output;
}

void set_log_output(int output)
{
    log_output = output;
}

#ifdef _LOG_TO_FILE_
#include <sys/stat.h>
#include <dirent.h>

#define FILE_NAME_LEN                       256
#define FILE_PATH_LEN                       256

static char log_file_name[FILE_NAME_LEN] = { 0 };

static int is_dir_exist(const char* dir_path)
{
    DIR* dir;

    if (dir_path == NULL)
        return 0;

    if ((dir = opendir(dir_path)) == NULL)
        return 0;
    else
        closedir(dir);

    return 1;
}

static void create_log_dir(void)
{
    char trace_dir[FILE_PATH_LEN];

    memset(trace_dir, 0, sizeof(trace_dir));
    sprintf(trace_dir, "%s/%s/", LOG_PATH, LOG_DIR);
    if (!is_dir_exist(trace_dir)) {
        mk_dir(trace_dir);
    }
    printf("Root path of logs:%s\n", trace_dir);
}

/**
 * Check log files if it is retains too long time
 * NOTE: It should be called periodically
 * @param retain_days
 */
void log_trim(u32 retain_days)
{
    DIR* dir;
    struct dirent* ptr;
    struct stat buf;
    int result;
    struct timeval tv;
    struct tm w;
    time_t timeBegin, timeEnd;
    char base[1000];
    char basePath[FILE_NAME_LEN];

    memset(basePath, 0, sizeof(basePath));
    snprintf(basePath, FILE_NAME_LEN, "%s/%s", LOG_PATH, LOG_DIR);

    gettimeofday(&tv, NULL);
    timeBegin = tv.tv_sec;
    timeEnd = timeBegin - retain_days * 24 * 3600;//retain logs for "retain_days" days.
    LOGI("current_log_file_name:%s", log_file_name);
    LOGI("Only retain logs for 15 days, cut range:timeBegin=%ld, timeEnd=%ld", timeBegin, timeEnd);
    if ((dir = opendir(basePath)) == NULL) {
        LOGI("Open dir error...");
    } else {
        while ((ptr = readdir(dir)) != NULL) {
            if (strcmp(ptr->d_name, ".") == 0 ||
                strcmp(ptr->d_name, "..") == 0)    ///current dir OR parent dir
                continue;
            else if (ptr->d_type == 8) {//file
                //CPE_LOG("fileName:%s/%s",basePath,ptr->d_name);
                memset(base, 0, sizeof(base));
                sprintf(base, "%s/%s", basePath, ptr->d_name);
                result = stat(base, &buf);
                if (result != 0) {
                    LOGI("list file info error.");//No such file or directory
                } else {
                    if (buf.st_ctime < timeEnd) {
                        remove(base);
                        LOGI("%s is deleted.", ptr->d_name);
                        gmtime_r(&buf.st_mtime, &w);
                        LOGI("Last Modify Time: %ld,%04d-%02d-%02d-%02d-%02d-%02d",
                             buf.st_ctime, w.tm_year + 1900, w.tm_mon + 1, w.tm_mday,
                             w.tm_hour, w.tm_min, w.tm_sec);
                    }
                }
            } else if (ptr->d_type == 10)    //link file
                LOGI("10,d_name:%s/%s", basePath, ptr->d_name);
            else if (ptr->d_type == 4)    //dir
                LOGI("4,d_name:%s/%s", basePath, ptr->d_name);
        }
        closedir(dir);
    }
}
#endif

static void log_init(void)
{
#ifdef _LOG_TO_FILE_
    create_log_dir();   //create directory
    memset(log_file_name, 0, sizeof(log_file_name));

    snprintf(log_file_name, FILE_NAME_LEN, "%s/%s/%s", LOG_PATH, LOG_DIR, LOG_FILE_NAME);
    printf("Log File Name: %s\n", log_file_name);
#endif

    log_is_init = true;
}

int log_raw(const char* fmt, ...)
{
    va_list list;
    char buffer[LOGS_BUFFER_LENGTH];

    if (!log_output)
        return 0;

    if (!log_is_init)
        log_init();

    va_start(list, fmt);
    vsnprintf(buffer, LOGS_BUFFER_LENGTH, fmt, list);
    va_end(list);

    printf("%s", buffer);
#ifdef _LOG_TO_FILE_
    FILE* log_file;

    log_file = fopen(log_file_name, "a+");

    if (log_file == NULL)
        return -1;

    fprintf(log_file, "%s\n", buffer);
    fclose(log_file);
#endif
    return 0;
}

int logs(const char* file, const char* func, u32 line, const char* LOG_COLOR, const char* fmt, ...)
{
    va_list list;
    struct date_time_str time;
    char buffer[LOGS_BUFFER_LENGTH] = { 0 };

    if (!log_output)
        return 0;

    if (!log_is_init)
        log_init();

    get_datetime_string(&time);

    va_start(list, fmt);
    vsnprintf(buffer, LOGS_BUFFER_LENGTH, fmt, list);
    va_end(list);

    printf("[%s]%s %s  line:%-4d  %s"LOG_RESET_COLOR"\n", time.string, LOG_COLOR, file, line, buffer);
#ifdef _LOG_TO_FILE_
    FILE* log_file;

    log_file = fopen(log_file_name, "a+");

    if (log_file == NULL)
        return -1;

    fprintf(log_file, "[%s]%s %s  line:%-4d  %s"LOG_RESET_COLOR"\n", time.string, LOG_COLOR, file, line, buffer);
    fclose(log_file);
#endif
    return 0;
}

int log_hex(const u8* arr, u32 arr_len, const char* file, u32 line, const char* fmt, ...)
{
    va_list list;
    struct date_time_str time;
    size_t i = 0, j = 0;
    char hex[PRINT_BUFFER_LENGTH] = { 0 };
    char notice[PRINT_BUFFER_LENGTH] = { 0 };

    if (!log_output)
        return 0;

    if (!log_is_init)
        log_init();

    get_datetime_string(&time);

    for (j = 0;j < PRINT_BUFFER_LENGTH - 3;) {
        if (i >= arr_len)
            break;
        snprintf(&hex[j], PRINT_BUFFER_LENGTH, "%02X ", arr[i++]);
        j += 3;
    }
    hex[j] = 0;

    va_start(list, fmt);
    vsnprintf(notice, LOGS_BUFFER_LENGTH, fmt, list);
    va_end(list);

    printf("[%s]"LOG_COLOR_I" %s  line:%-4d  %s%s"LOG_RESET_COLOR"\n", time.string, file, line, notice, hex);

#ifdef _LOG_TO_FILE_
    FILE* log_file;

    log_file = fopen(log_file_name, "a+");

    if (log_file == NULL)
        return -1;

    fprintf(log_file, "[%s]"LOG_COLOR_I" %s  line:%-4d  %s%s"LOG_RESET_COLOR"\n", time.string, file, line, notice, hex);
    fclose(log_file);
#endif

    return 0;
}
