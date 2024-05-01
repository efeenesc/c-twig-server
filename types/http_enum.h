#ifndef http_enum_h
#define http_enum_h

typedef enum {
  HTTP_GET,
  HTTP_POST,
  HTTP_PUT,
  HTTP_PATCH,
  HTTP_DELETE,
  HTTP_HEAD,
  HTTP_OPTIONS,
  HTTP_UNKNOWN
} HttpRequestType;

HttpRequestType str_to_http_request_enum(char *input);

#endif