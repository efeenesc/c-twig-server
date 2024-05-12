#include "json.h"

#include <ctype.h>
#include <locale.h>
#include <sec_api/string_s.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "types/json_types.h"

JsonLinkedList *json_parse(const char **targetString) {
  JsonLinkedList *result = NULL;
  while (**targetString != '\0' && *(*targetString) != '\0') {
    if (**targetString == '{') {
      ++(*targetString);
      result = json_obj_parse(targetString, false);
    } else {
      ++(*targetString);
    }
  }
  return result;
}

JsonLinkedList *json_obj_parse(const char **targetString, bool inArray) {
  JsonLinkedList *thisObj = malloc(sizeof(JsonLinkedList));
  thisObj->next = NULL;

  JsonLinkedList *head = thisObj;
  JsonLinkedList *prev = NULL;

  bool keySet = false;

  // Count writes and what the next target in the array is in value_buff with
  // buff_write_cnt, reset array with memset and buff_write_cnt after copying
  char valueBuff[8192];
  memset(valueBuff, 0, sizeof(valueBuff));
  // Reset buff_write_cnt after every copy of value_buff to another array
  unsigned short buffWriteCnt = 0;
  bool readingString = false;

  unsigned short arrayMemberCnt = 0;
  bool readingArray = false;
  bool escapeCharacter = false;

  // Read while target string is not currently null terminated
  while (**targetString != '\0') {
    /*
      Read everything between quotation marks.
      If there is no key, this is likely a key; capture it and copy it to .key
      If there is a key, this is likely a value. Capture it and copy it to
      .value
    */

    if (**targetString == '\\') {
      escapeCharacter = true;
    }

    if (!readingString) {
      if (**targetString == '[') {
        if (keySet) {
          readingArray = true;
          ++(*targetString);

          JsonValue **newArrayMember = malloc(sizeof(JsonValue *) * 1);
          unsigned short size = json_get_array(targetString, &newArrayMember);

          thisObj->value = malloc(sizeof(JsonValue));
          thisObj->value->valueType = JSON_ARR;
          thisObj->value->arrVal = newArrayMember;
          thisObj->value->_arrSize = size;

          prev = thisObj;
          thisObj = NULL;
          keySet = false;
        } else {
          printf("Key is not set for array value! Returning NULL\n");
          return NULL;
        }
      }

      if (**targetString == ']') {
        if (!readingArray && !inArray)
          return NULL;
        readingArray = false;
        // return thisObj;
      }
    }

    if (**targetString == '\"') {
      if (escapeCharacter) {
        escapeCharacter = false;
      } else if (!readingString) {
        readingString = true;
        ++(*targetString);
        continue;
      } else {
        readingString = false;

        if (thisObj == NULL) {
          thisObj = malloc(sizeof(JsonLinkedList));
          thisObj->next = NULL;
          prev->next = thisObj;
        }

        if (keySet) {
          thisObj->value = malloc(sizeof(JsonValue));
          thisObj->value->valueType = JSON_STR;
          thisObj->value->strVal = malloc(sizeof(char) * buffWriteCnt + 1);

          strcpy_s(thisObj->value->strVal, buffWriteCnt + 1, valueBuff);

          prev = thisObj;
          thisObj = NULL;
          keySet = false;
        } else {
          thisObj->key = malloc(buffWriteCnt);
          strcpy_s(thisObj->key, buffWriteCnt + 1, valueBuff);
          keySet = true;
        }

        memset(valueBuff, 0, buffWriteCnt);
        buffWriteCnt = 0;
        ++(*targetString);
        continue;
      }
    } else if (**targetString == '}') {
      ++(*targetString);
      break;
    } else if (**targetString == '{') {
      ++(*targetString);
      JsonLinkedList *childObject = json_obj_parse(targetString, false);
      //printf("CREATING NEW OBJ\n");
      thisObj->value = malloc(sizeof(JsonValue));
      thisObj->value->valueType = JSON_OBJ;
      thisObj->value->objVal = childObject;
    } else if (readingString) {
      valueBuff[buffWriteCnt] = **targetString;
      buffWriteCnt++;
    } else {
      if (!isspace(**targetString)) {
        // printf("Weird character found: %c\n", **targetString);
      }
    }

    ++(*targetString);
  };

  return head;
}

unsigned short json_get_array(const char **targetString, JsonValue ***array) {
  unsigned short arrayMemberCnt = 0;

  bool readingString = false;
  bool readingNumber = false;

  char buffer[1024];
  memset(buffer, 0, sizeof(buffer));
  unsigned short charReadCnt = 0;

  while (**targetString != '\0' && **targetString != ']') {
    if (**targetString == '{' && !readingString) {
      ++(*targetString);
      JsonLinkedList *result = json_obj_parse(targetString, false);
      unsigned short count = json_count(result);

      JsonValue *newValue = malloc(sizeof(JsonValue));

      newValue->valueType = JSON_OBJ;
      newValue->objVal = result;

      *array = realloc(*array, sizeof(JsonValue *) * (arrayMemberCnt + 1));

      (*array)[arrayMemberCnt] = newValue;
      arrayMemberCnt++;
    }

    if (isdigit(**targetString) && !readingString) {
      JsonValueType type;
      void *result = json_get_number(targetString, &type);

      JsonValue *newValue = malloc(sizeof(JsonValue));

      *array = realloc(*array, sizeof(JsonValue *) * (arrayMemberCnt + 1));

      (*array)[arrayMemberCnt] = newValue;
      arrayMemberCnt++;

      switch (type) {
      case JSON_INT:
        newValue->valueType = JSON_INT;
        newValue->intVal = (int)(intptr_t)result;
        break;
      case JSON_SHORT:
        newValue->valueType = JSON_SHORT;
        newValue->shortVal = (short)(intptr_t)result;
        break;
      case JSON_LONG:
        newValue->valueType = JSON_LONG;
        newValue->longVal = (long)(intptr_t)result;
        break;
      default:
        break;
      }
    }

    if (**targetString == '\"') {
      if (readingString) {
        readingString = false;

        JsonValue *newValue = malloc(sizeof(JsonValue));

        newValue->valueType = JSON_STR;
        newValue->strVal = malloc(charReadCnt + 1);

        strcpy(newValue->strVal, buffer);
        memset(buffer, 0, sizeof(buffer));
        charReadCnt = 0;

        *array = realloc(*array, sizeof(JsonValue *) * (arrayMemberCnt + 1));

        (*array)[arrayMemberCnt] = newValue;
        arrayMemberCnt++;
      } else {
        readingString = true;
      }
    } else {
      if (readingString) {
        buffer[charReadCnt] = **targetString;
        charReadCnt++;
      }
    }

    ++(*targetString);
  }

  return arrayMemberCnt;
}

void *json_get_number(const char **targetString, JsonValueType *type) {
  long long numberRead = 0;
  char *endptr;

  while (**targetString != '\0' && isdigit(**targetString)) {
    numberRead = (numberRead * 10) + (**targetString - '0');
    (*targetString)++;
  }

  if (numberRead >= INT_MIN && numberRead <= INT_MAX) {
    *type = JSON_INT;
    return (void *)(intptr_t)numberRead;
  } else if (numberRead >= SHRT_MIN && numberRead <= SHRT_MAX) {
    *type = JSON_SHORT;
    return (void *)(intptr_t)numberRead;
  } else if (numberRead >= LONG_MIN && numberRead <= LONG_MAX) {
    *type = JSON_LONG;
    return (void *)(intptr_t)numberRead;
  }

  return NULL;
}

void json_free(JsonLinkedList *toDelete) {
  while (toDelete != NULL) {
    JsonLinkedList *temp = toDelete;
    toDelete = toDelete->next;

    if (temp->key != NULL) {
      // free(temp->key);
      temp->key = NULL;
    }

    if (temp->value != NULL) {
      switch (temp->value->valueType) {
      case JSON_OBJ:
        json_free(temp->value->objVal);
        temp->value->objVal = NULL;
        break;

      case JSON_STR:
        free(temp->value->strVal);
        temp->value->strVal = NULL;
        break;

      case JSON_INT:
      case JSON_LONG:
      case JSON_ARR:
        json_free_array(&temp->value->arrVal, temp->value->_arrSize);
        temp->value->arrVal = NULL;
        break;

      case JSON_UNDEFINED:
      case JSON_SHORT:
      case JSON_NULL:
      case JSON_UNKNOWN:
        break;
      }

      free(temp->value);
      temp->value = NULL;
    }

    free(temp);
    temp = NULL;
  }
}

void json_free_array(JsonValue ***arr, unsigned short size) {
  JsonValue **array = *arr;

  for (int i = 0; i < size; i++) {
    if (array[i] == NULL) {
      printf("WTF\n");
      continue;
    }

    if (array[i]->valueType != JSON_UNKNOWN) {
      switch (array[i]->valueType) {
      case JSON_OBJ:
        json_free(array[i]->objVal);
        array[i]->objVal = NULL;
        break;

      case JSON_STR:
        free(array[i]->strVal);
        array[i]->strVal = NULL;
        break;

      case JSON_ARR:
        json_free_array(&array[i]->arrVal, array[i]->_arrSize);
        array[i]->arrVal = NULL;
        break;

      case JSON_INT:
      case JSON_LONG:
      case JSON_UNDEFINED:
      case JSON_SHORT:
      case JSON_NULL:
      case JSON_UNKNOWN:
        break;
      }

      free(array[i]);
      array[i] = NULL;
    }
  }

  free(*arr);
}


void *json_find_key(JsonLinkedList *targetObj, char *targetKey) {
  JsonLinkedList *search = targetObj;

  while (search != NULL) {
    if (strcmp(search->key, targetKey) == 0) {
      return search;
    }
    search = search->next;
  }

  return NULL;
}

void json_iterate(JsonLinkedList *targetObj) {
  JsonLinkedList *i = targetObj;

  while (i != NULL) {

    if (i->value == NULL) {
      printf("FUCK\n");
      continue;
    }
    switch (i->value->valueType) {
    case JSON_STR:
      printf("Key: %s\n", i->key);
      printf("Value: %s\n", i->value->strVal);
      break;

    case JSON_OBJ:
      printf("Key: %s\n", i->key);
      json_iterate(i->value->objVal);
      break;

    case JSON_INT:
    case JSON_LONG:
    case JSON_ARR:
      printf("Key: %s\n", i->key);
      json_iterate_array(i->value->arrVal, i->value->_arrSize);
      break;

    case JSON_UNDEFINED:
    case JSON_NULL:
    case JSON_UNKNOWN:
    case JSON_SHORT:
      break;
    }

    i = i->next;
  }
}

void json_iterate_array(JsonValue **arr, unsigned short size) {
  for (int i = 0; i < size; i++) {
    if (arr[i] == NULL) {
      printf("WTF\n");
      return;
    }

    JsonValue *value = arr[i];


    switch (value->valueType) {
    case JSON_INT:
      printf("Integer: %d\n", value->intVal);
      break;

    case JSON_SHORT:
      printf("Short: %d\n", value->shortVal);
      break;

    case JSON_LONG:
      printf("Long: %ld\n", value->longVal);
      break;

    case JSON_STR:
      printf("String: %s\n", value->strVal);
      break;

    case JSON_ARR:
      json_iterate_array(value->arrVal, value->_arrSize);
      break;

    case JSON_OBJ:
      json_iterate(value->objVal);
      break;

    case JSON_UNDEFINED:
    case JSON_NULL:
    case JSON_UNKNOWN:
      break;
    }
  }
}

unsigned short json_count(JsonLinkedList *targetObj) {
  JsonLinkedList *i = targetObj;
  unsigned short count = 0;

  while (i != NULL) {
    count++;
    i = i->next;
  }

  return count;
}