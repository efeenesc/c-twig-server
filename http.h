#ifndef http_h
#define http_h

int handle_http(char buffer[]);
int match_path(const char *requestType, const char *requestPath);

#endif