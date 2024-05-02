/***********************************************************************
 * @file main.c
 * @brief 
 * Author: Linlin Zhou
 * E-mail: 461146760@qq.com
 * Date: 2024-05-01
 * @Copyright (C) 2024 all right reserved
***********************************************************************/

#include "logs.h"
#include <stdlib.h>

#define COLORFUL_LOG(fmt, ...) \
    do { \
        LOGI(fmt , ##__VA_ARGS__); \
        LOGW(fmt , ##__VA_ARGS__); \
        LOGE(fmt , ##__VA_ARGS__); \
        LOGD(fmt , ##__VA_ARGS__); \
        LOGV(fmt , ##__VA_ARGS__); \
    } while (0)

int main(int argc, char** argv)
{
    COLORFUL_LOG("%s, %S", getenv("LPAC_APDU"), getenv("LPAC_APDU"));

    return 0;
}