/***********************************************************************
 * @file common_json.h
 * @brief  COMMON_JSON
 * Author: Linlin Zhou
 * E-mail: 461146760@qq.com
 * Date: 2024-05-02
 * @Copyright (C) 2024 all right reserved
***********************************************************************/
#ifndef __COMMON_JSON_H__
#define __COMMON_JSON_H__

#include <stdint.h>
#include "parson.h"
#include "logs.h"

#ifdef __cplusplus
extern "C"{
#endif

const char *obj_get_string(JSON_Object *obj, const char *name);
double obj_get_number(JSON_Object *obj, const char *name);
void obj_remove(JSON_Object *obj, const char *name);
void obj_update_string(JSON_Object *obj, const char *name);
void obj_update_number(JSON_Object *obj, const char *name);
void obj_update_bool(JSON_Object *obj, const char *name);

JSON_Object *json_object_create(void);
void json_object_destroy(JSON_Object *object);
JSON_Object *json_object_create_by_str(const char *json_string);

void json_print_raw(const char *file, uint32_t line, JSON_Object *obj, const char *fmt, ...);
#define json_print(obj, ...) json_print_raw(__FILENAME__, __LINE__, obj, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif // __COMMON_JSON_H__
