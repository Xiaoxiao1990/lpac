/*********************************************************************
 * Author: Zhou Linlin
 * E-mail: 461146760@qq.com
 * Date  : 2022-06-10 10:08:21.
 * Copyright (c) 2022 all rights reserved.
*********************************************************************/

#ifndef __LOG_H__
#define __LOG_H__

#include "types.h"
#include <string.h>

#define LOGS_BUFFER_LENGTH             2048
#define PRINT_BUFFER_LENGTH            2048

#define LOG_COLOR_BLACK   "30"
#define LOG_COLOR_RED     "31"
#define LOG_COLOR_YELLOW  "32"
#define LOG_COLOR_BROWN   "33"
#define LOG_COLOR_BLUE    "34"
#define LOG_COLOR_PURPLE  "35"
#define LOG_COLOR_CYAN    "36"
#define LOG_COLOR(COLOR)  "\033[0;" COLOR "m"
#define LOG_BOLD(COLOR)   "\033[1;" COLOR "m"
#define LOG_RESET_COLOR   "\033[0m"
#define LOG_COLOR_E       LOG_COLOR(LOG_COLOR_RED)
#define LOG_COLOR_W       LOG_COLOR(LOG_COLOR_BROWN)
#define LOG_COLOR_I       LOG_COLOR(LOG_COLOR_YELLOW)
#define LOG_COLOR_D
#define LOG_COLOR_V

#ifdef __cplusplus
extern "C"{
#endif

/**
 * A functional option of whether write the logs info a file or not.
 */
// #define _LOG_TO_FILE_
#ifdef _LOG_TO_FILE_
#define LOG_PATH                "/data/showmac"
#define LOG_DIR                 "logs"
#define LOG_FILE_NAME			"showmac.log"

/**
 * Check log files if it is retains too long time
 * NOTE: It should be called periodically
 * @param retain_days
 */
void log_trim(u32 retain_days);

#endif

int get_log_output(void);

void set_log_output(int output);

int log_raw(const char *fmt, ...);

int logs(const char *file, const char *func, u32 line, const char *LOG_COLOR, const char *fmt, ...);

int log_hex(const u8 *arr, u32 arr_len, const char *file, u32 line, const char *fmt, ...);

#define CONSTANT_STRING_DEFINE(s)  const char * const s##_CONST_STRING = #s
#define CONSTANT_STRING_DECLARE(s)  extern const char * const s##_CONST_STRING

#define __FILENAME__                (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1):__FILE__)
#define get_file_name(s)            (strrchr(s, '/') ? (strrchr(s, '/') + 1):(s))

#define LOGI(...)                   logs(__FILENAME__, __func__, __LINE__, LOG_COLOR_I, ##__VA_ARGS__)
#define LOGE(...)                   logs(__FILENAME__, __func__, __LINE__, LOG_COLOR_E, ##__VA_ARGS__)
#define LOGW(...)                   logs(__FILENAME__, __func__, __LINE__, LOG_COLOR_W, ##__VA_ARGS__)
#define LOG_HEX(_arr, _len, ...)    log_hex(_arr, _len, __FILENAME__ , __LINE__, ##__VA_ARGS__)
#define LOG_MARK        LOGI("")

#ifdef __cplusplus
}
#endif

#endif /* __LOG_H__ */
