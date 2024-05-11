#include "utils/hashmap.h"
#include "types/http_types.h"

#ifndef http_h
#define http_h

int handle_http(char buffer[], void **response, unsigned long long *responseSize);
int match_path(const char *requestType, const char *requestPath);
int handle_options(void **response, unsigned long long *responseSize, struct hashmap *headers, HttpResponse *toWrite);
int write_response(void **response, unsigned long long *responseSize, struct hashmap *headers, HttpResponse *toWrite);

#endif