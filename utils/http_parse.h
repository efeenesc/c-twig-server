#include "hashmap.h"

#ifndef http_parse_h
#define http_parse_h

int parse_http(char request[], struct hashmap **p_headerMap, void **bodyResultObj);
int read_http_header(char request[], struct hashmap *header_map, const char **request_reader);
int check_request_type(struct hashmap *header_map);
int parse_body(char request[], struct hashmap *p_headerMap, const char **p_requestReader, void **output);

#endif