#ifndef json_types_h
#define json_types_h

typedef enum {
  JSON_INT,
  JSON_SHORT,
  JSON_LONG,
  JSON_STR,
  JSON_ARR,
  JSON_OBJ,
  JSON_UNDEFINED,
  JSON_NULL,
  JSON_UNKNOWN
} JsonValueType;

typedef struct JsonValue JsonValue;
struct JsonValue {
  JsonValueType valueType;
  unsigned short _arrSize;
  union {
    int intVal;
    short shortVal;
    long longVal;
    char *strVal;
    JsonValue **arrVal;
    struct JsonLinkedList *objVal;
  };
};

typedef struct JsonLinkedList JsonLinkedList;
struct JsonLinkedList {
  char *key;
  JsonValue *value;
  JsonLinkedList *next;
};

#endif