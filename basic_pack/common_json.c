//
// Created by smallfish on 6/29/21.
//

#include <parson.h>
#include <logs.h>
#include <stdio.h>
#include <stdarg.h>
#include "common_json.h"

void json_print_raw(const char *file, uint32_t line, JSON_Object *obj, const char *fmt, ...)
{
    va_list list;
    char buffer[LOGS_BUFFER_LENGTH] = {0};
    char *msg = json_serialize_to_string_pretty(json_object_get_wrapping_value(obj));

    va_start(list, fmt);
    vsnprintf(buffer, LOGS_BUFFER_LENGTH, fmt, list);
    va_end(list);

    logs(file, "", line, LOG_COLOR_I, "%s%s", buffer, msg);

    json_free_serialized_string(msg);
}

const char *obj_get_string(JSON_Object *obj, const char *name)
{
    if (json_object_has_value(obj, name))
        return json_object_get_string(obj, name);
    else
        return "";
}


double obj_get_number(JSON_Object *obj, const char *name)
{
    if (json_object_has_value(obj, name))
        return json_object_get_number(obj, name);
    else
        return -1;
}

///////////// common
JSON_Object *json_object_create(void)
{
    JSON_Value *value = json_value_init_object();
    if (!value) {
        LOGE("");
        return NULL;
    }

    return json_object(value);
}

JSON_Object *json_object_create_by_str(const char *json_string)
{
    JSON_Value *value = json_parse_string_with_comments(json_string);
    if (!value) {
        LOGE("");
        return NULL;
    }
    JSON_Object *object = json_object(value);
    if (object == NULL) {
        LOGE("");
        json_value_free(value);
        return NULL;
    }
    return object;
}

void json_object_destroy(JSON_Object *object)
{
    if (object) {
        json_value_free(json_object_get_wrapping_value(object));
        object = NULL;
    }
}
