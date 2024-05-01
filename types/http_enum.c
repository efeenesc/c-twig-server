#include "http_enum.h"
#include <string.h>
#include <stdio.h>

HttpRequestType str_to_http_request_enum(char *input) {
  if (strcmp(input, "GET") == 0) {
    return HTTP_GET;
  } else if (strcmp(input, "POST") == 0) {
    return HTTP_POST;
  } else if (strcmp(input, "PUT") == 0) {
    return HTTP_PUT;
  } else if (strcmp(input, "PATCH") == 0) {
    return HTTP_PATCH;
  } else if (strcmp(input, "DELETE") == 0) {
    return HTTP_DELETE;
  } else if (strcmp(input, "HEAD") == 0) {
    return HTTP_HEAD;
  } else if (strcmp(input, "OPTIONS") == 0) {
    return HTTP_OPTIONS;
  } else {
    return HTTP_UNKNOWN;
  };
}