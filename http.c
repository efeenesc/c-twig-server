#include "http.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <sec_api/string_s.h>

#include "files.h"
#include "json.h"

#include "types/http_enum.h"
#include "types/key_value_pair.h"
#include "utils/string_util.h"
#include "utils/hashmap.h"
#include "utils/http_parse.h"

int handle_http(char buffer[]) {
  struct hashmap *http_headers = NULL;
  void *http_body = NULL;
  int result = parse_http(buffer, &http_headers, &http_body);
  
  if (result != 0) {
    printf("Could not parse body\n");
    return 1;
  }

  const char *requestType = get_value_from_key(http_headers, "type");
  const char *requestPath = get_value_from_key(http_headers, "path");

  match_path(requestType, requestPath);

  json_free(http_body);
  hashmap_free(http_headers);

  return 0;
}

int match_path(const char *requestType, const char *requestPath) {
  // Split the requested path string to tokenize paths for matching
  char **splitArray = NULL;
  unsigned short arrSize = strsplit(&requestPath, '/', &splitArray);

  char *folderPath = NULL;
  strjoin(&splitArray, 0, arrSize-2, '/', &folderPath);

  printf("Result: %s\n", folderPath);

  HttpRequestType type = str_to_http_request_enum((char*)requestType);

  if (type == HTTP_GET) {
    if (check_dir(folderPath) == 0) {
      //! This is NOT optimal and is vulnerable code. Fix later!
      if (check_file(requestPath)) {
        printf("File found\n");
      } else {
        printf("File not found\n");
      }
    } else {
      printf("Path not found\n");
    }
  };

  strsplit_free(&splitArray, arrSize);
  free(folderPath);
  return 0;
}