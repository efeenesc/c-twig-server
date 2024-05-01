#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <sec_api/string_s.h>
#include "http.h"
#include "utils/string_util.h"
#include "types/key_value_pair.h"
#include "utils/hashmap.h"
#include "types/http_enum.h"

#define HTTP_MIN_SIZE 10

int parse_http(char request[]) {
  const char* p_requestReader = request;
  struct hashmap *p_headerMap = hashmap_new(sizeof(char[127]), 0, 0, 0, dict_hash, dict_compare, NULL, NULL);
  int result = read_http_header(request, p_headerMap, p_requestReader);
  if (result != 0) {
    printf("Error reading header");
    return 1;
  }

  HttpRequestType requestType = check_request_type(p_headerMap);
  if (requestType == HTTP_UNKNOWN) {
    printf("Unknown HTTP request type");
    return 1;
  }

  int bodyResult = -1;

  switch (requestType) {
    case HTTP_POST:
    case HTTP_PUT:
    case HTTP_PATCH:
    case HTTP_DELETE:
    case HTTP_HEAD:
    case HTTP_OPTIONS:
      bodyResult = parse_body(request, p_headerMap, p_requestReader);
      break;
    default:
      break;
  }

  return 0;
}

int read_http_header(char request[], struct hashmap *p_headerMap, const char *p_requestReader) {
  int requestLen = strlen(request);
  char buff[255];
  
  if (requestLen < HTTP_MIN_SIZE) {
    return 1;
  }

  int line_count = 0;

  while (NULL != sgets(buff, sizeof(buff), &p_requestReader)) {
    char* temp_str = strdup(buff);

    if (line_count == 0) {
      // Extract request type, path, and HTTP version
      char* httpRequestType = strtok(temp_str, " ");
      // printf("Request type: %s\n", trim(httpRequestType));

      char* httpRequestPath = strtok(NULL, " ");
      // printf("Request path: %s\n", trim(httpRequestPath));

      char* httpRequestVersion = strtok(NULL, " ");
      // printf("HTTP Version: %s\n", trim(httpRequestVersion));
      
      // Add "type" and requested type to dictionary
      KeyValPair newPair;
      newPair.key = (char*)"type";
      newPair.value = trim(httpRequestType);

      hashmap_set(p_headerMap, &newPair);

      // Add "path" and requested path to dictionary
      KeyValPair newPair1;
      newPair1.key = (char*)"path";
      newPair1.value = trim(httpRequestPath);

      hashmap_set(p_headerMap, &newPair1);
    } else {
      char* key = strtok(temp_str, ":");
      char* val = strtok(NULL, "");
      // printf("Key: %s\n", trim(key));
      // printf("Value: %s\n", trim(val));

      if (strlen(key) <= 0 || strlen(val) <= 0) continue;

      char *trimmedKey = trim(key);

      KeyValPair newPair;
      newPair.key = trimmedKey;
      newPair.value = trim(val);
      
      hashmap_set(p_headerMap, &newPair);
    }
    
    line_count++;
  }

  // Increment the pointer to push it forward into the request body
  p_requestReader++;
  return 0;
}

int check_request_type(struct hashmap *p_headerMap) {
  const char *result = get_value_from_key(p_headerMap, "type");

  if (result == NULL) {
    printf("Key not found in hashmap\n");
    return HTTP_UNKNOWN;
  }

  HttpRequestType requestType = str_to_http_request_enum((char*)result);
  
  return requestType;
}

int parse_body(char request[], struct hashmap *p_headerMap, const char *p_requestReader) {
  const char *result = get_value_from_key(p_headerMap, "Content-Type");


  if (result == NULL) {
    printf("Content-Type was not found\n");
    return 1;
  }

  if (strcmp(result, "application/json") == 0) {
    printf("JSON formatted body\n");
  }

  printf("%s\n",result);

  return 0;
}