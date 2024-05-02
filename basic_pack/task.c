/**
 * Copyright (c) 2018 TGT, Inc. All Rights Reserved.
 * Date   : 9/20/18 3:04 PM
 * Author : Zhou Linlin
 * e-mail : 461146760@qq.com
 * */

#include "task.h"
#include "logs.h"

#include <pthread.h>
#include <string.h>

int task_maker(task_func_t task, void *task_arg)
{
    int result = 0;
    pthread_t pid;

    result = pthread_create(&pid, NULL, task, task_arg);

    if (0 != result) {
        LOGI("create task failure");
        return -1;
    }

    pthread_detach(pid);

    return result;
}
