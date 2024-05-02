#include "jprint.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <common_json.h>

static void json_print_pretty(const char* msg)
{
    JSON_Value* root_value = json_parse_string(msg);
    if (root_value == NULL) {
        return;
    }

    JSON_Object* obj = json_object(root_value);

    
}

void jprint_error(const char* function_name, const char* detail)
{
    cJSON *jroot = NULL;
    cJSON *jpayload = NULL;
    char *jstr = NULL;

    if (detail == NULL)
    {
        detail = "";
    }

    jroot = cJSON_CreateObject();
    cJSON_AddStringOrNullToObject(jroot, "type", "lpa");
    jpayload = cJSON_CreateObject();
    cJSON_AddNumberToObject(jpayload, "code", -1);
    cJSON_AddStringOrNullToObject(jpayload, "message", function_name);
    cJSON_AddStringOrNullToObject(jpayload, "data", detail);
    cJSON_AddItemToObject(jroot, "payload", jpayload);

    jstr = cJSON_PrintUnformatted(jroot);
    cJSON_Delete(jroot);

    // printf("%s\r\n", jstr);

    JSON_Object* obj = json_object_create_by_str(jstr);
    json_print(obj, "error: ");
    json_object_destroy(obj);

    fflush(stdout);
    free(jstr);
}

void jprint_progress(const char *function_name)
{
    cJSON *jroot = NULL;
    cJSON *jpayload = NULL;
    char *jstr = NULL;

    jroot = cJSON_CreateObject();
    cJSON_AddStringOrNullToObject(jroot, "type", "progress");
    jpayload = cJSON_CreateObject();
    cJSON_AddNumberToObject(jpayload, "code", 0);
    cJSON_AddStringOrNullToObject(jpayload, "message", function_name);
    cJSON_AddNullToObject(jpayload, "data");
    cJSON_AddItemToObject(jroot, "payload", jpayload);

    jstr = cJSON_PrintUnformatted(jroot);
    cJSON_Delete(jroot);

    // printf("%s\r\n", jstr);
    JSON_Object* obj = json_object_create_by_str(jstr);
    json_print(obj, "progress: ");
    json_object_destroy(obj);

    fflush(stdout);
    free(jstr);
}

void jprint_success(cJSON *jdata)
{
    cJSON *jroot = NULL;
    cJSON *jpayload = NULL;
    char *jstr = NULL;

    jroot = cJSON_CreateObject();
    cJSON_AddStringOrNullToObject(jroot, "type", "lpa");
    jpayload = cJSON_CreateObject();
    cJSON_AddNumberToObject(jpayload, "code", 0);
    cJSON_AddStringOrNullToObject(jpayload, "message", "success");
    if (jdata)
    {
        cJSON_AddItemToObject(jpayload, "data", jdata);
    }
    else
    {
        cJSON_AddNullToObject(jpayload, "data");
    }
    cJSON_AddItemToObject(jroot, "payload", jpayload);

    jstr = cJSON_PrintUnformatted(jroot);
    cJSON_Delete(jroot);

    // printf("%s\r\n", jstr);
    JSON_Object* obj = json_object_create_by_str(jstr);
    json_print(obj, "success: ");
    json_object_destroy(obj);

    fflush(stdout);
    free(jstr);
}
