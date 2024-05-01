//
// Created by smallfish on 6/29/21.
//

#ifndef LINUX_CPE_COMMON_JSON_H
#define LINUX_CPE_COMMON_JSON_H

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

#endif //LINUX_CPE_COMMON_JSON_H
