#include "utils/hashmap.h"

#ifndef http_h
#define http_h

int parse_http(char request[]);
int read_http_header(char request[], struct hashmap *header_map, const char *request_reader);
int check_request_type(struct hashmap *header_map);
int parse_body(char request[], struct hashmap *p_headerMap, const char *p_requestReader);

#endif