#include "http.h"

#include <ctype.h>
#include <sec_api/string_s.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../files.h"
#include "../json/json.h"

#include "../types/key_value_pair.h"
#include "../utils/hashmap.h"
#include "../utils/string_util.h"
#include "types/http_types.h"
#include "utils/http_parse.h"

int handle_http(char buffer[], void **response,
                unsigned long long *responseSize) {
  struct hashmap *http_headers = NULL;
  void *http_body = NULL;
  int result = parse_http(buffer, &http_headers, &http_body);

  if (result != 0) {
    printf("Could not parse body\n");
    return 1;
  }

  // Set a default error response
  HttpResponse res = {INTERNAL_SERVER_ERROR, TEXT_PLAIN, 0, 0};

  const char *requestTypeStr = get_value_from_key(http_headers, "type");
  const char *requestPathStr = get_value_from_key(http_headers, "path");

  char *file_buff = NULL;
  char *fullFolderPath = NULL;
  long fileSize = 0;
  fullFolderPath = get_full_path((char *)requestPathStr);
  HttpRequestType requestType = str_to_http_request_enum((char *)requestTypeStr);

  if (requestType == HTTP_OPTIONS) {
    printf("Handling options\n");
    handle_options(response, responseSize, http_headers, &res);
    return 0;
  }

  int pathMatchResult = match_path(requestTypeStr, requestPathStr);

  switch (pathMatchResult) {
  case 0:
    read_file_bytes(fullFolderPath, &file_buff, &fileSize);
    res.content = file_buff;
    res.contentLength = fileSize;
    res.responseCode = OK;
    break;
  case -1:
    read_file_bytes(strcat(fullFolderPath, "index.html"), &file_buff,
                    &fileSize);
    res.content = file_buff;
    res.contentLength = fileSize;
    res.responseCode = OK;
    break;
  default:
    res.content = NULL;
    res.contentLength = 0;
    res.responseCode = NOT_FOUND;
    break;
  }

  json_free(http_body);
  hashmap_free(http_headers);
  free(fullFolderPath);

  write_response(response, responseSize, http_headers, &res);

  return 0;
}

int match_path(const char *requestType, const char *requestPath) {
  char *fullFolderPath = NULL;
  fullFolderPath = get_full_path((char *)requestPath);

  HttpRequestType type = str_to_http_request_enum((char *)requestType);

  if (type == HTTP_GET) {
    char *fileExtension = str_reverse_search(fullFolderPath, '.');
    if (fileExtension != NULL) {
      if (check_file(fullFolderPath) == 0) {
        printf("File found\n");
        free(fullFolderPath);
        return 0;
      } else {
        printf("File not found: %s\n", fullFolderPath);
      }
    } else {
      if (check_file(strcat(fullFolderPath, "index.html")) == 0) {
        printf("Alternate file\n");
        free(fullFolderPath);
        return -1;
      }
    }
  };

  free(fullFolderPath);
  return 1;
}

int handle_options(void **response, unsigned long long *responseSize,
                   struct hashmap *headers, HttpResponse *toWrite) {
  char *statusText = http_response_code_to_str(toWrite->responseCode);
  char buffer[1024];
  memset(buffer, 0, sizeof(buffer));

  snprintf(buffer, sizeof(buffer),
           "HTTP/1.1 %s\r\n"
           "Allow: GET\r\n",
           statusText);

  if (*response == NULL) {
    unsigned long long resLen = strlen(buffer) + toWrite->contentLength;
    *response = malloc(sizeof(char) * resLen);
    if (*response == NULL) {
      return -1;
    }

    memcpy(*response, buffer, strlen(buffer));
    memcpy(*response + strlen(buffer), toWrite->content,
           toWrite->contentLength);

    *responseSize = resLen;
  }

  return 0;
}

int write_response(void **response, unsigned long long *responseSize,
                   struct hashmap *headers, HttpResponse *toWrite) {
  char *statusText = http_response_code_to_str(toWrite->responseCode);
  char buffer[1024];
  memset(buffer, 0, sizeof(buffer));

  snprintf(buffer, sizeof(buffer),
           "HTTP/1.1 %s\r\n"
           "Server: twig-server\r\n"
           "Content-Type: text-html\r\n"
           "Date: Sat, 4 May 2024 09:23:25 GMT\r\n"
           "Content-Length: %llu\r\n\r\n",
           statusText, toWrite->contentLength);

  if (*response == NULL) {
    unsigned long long resLen = strlen(buffer) + toWrite->contentLength;
    *response = malloc(sizeof(char) * resLen);
    if (*response == NULL) {
      return -1;
    }

    memcpy(*response, buffer, strlen(buffer));
    memcpy(*response + strlen(buffer), toWrite->content,
           toWrite->contentLength);

    *responseSize = resLen;
  }

  return 0;
}