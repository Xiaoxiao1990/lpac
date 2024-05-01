/**
 * Copyright (c) 2018 TGT, Inc. All Rights Reserved.
 * Date   : 9/20/18 3:04 PM
 * Author : Zhou Linlin
 * e-mail : 461146760@qq.com
 * */

#ifndef TASK_H
#define TASK_H

#ifdef __cplusplus
extern "C"{
#endif

typedef void *(*task_func_t)(void *);

int task_maker(task_func_t task, void *task_arg);

#ifdef __cplusplus
}
#endif
#endif // TASK_H
