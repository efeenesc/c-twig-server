#ifndef json_h
#define json_h

#include "types/json_types.h"
#include <stdbool.h>

JsonLinkedList *json_parse(const char **targetString);
JsonLinkedList *json_obj_parse(const char **targetString, bool inArray);
unsigned short json_get_array(const char **targetString, JsonValue ***array);
void *json_get_number(const char **targetString, JsonValueType *type);
void json_free(JsonLinkedList *toDelete);
void json_free_array(JsonValue ***arr, unsigned short size);
void *json_find_key(JsonLinkedList *targetObj, char *targetKey);
void json_iterate(JsonLinkedList *targetObj);
void json_iterate_array(JsonValue **arr, unsigned short size);
unsigned short json_count(JsonLinkedList *targetObj);

#endif