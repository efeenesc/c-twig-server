#ifndef tls_h
#define tls_h

#include "types/tls_types.h"
#include <winsock2.h>

int initiate_tls_call(char *buf, int len, SOCKET sock);
void *create_server_hello(TLSClientRecord *cHello);
void generate_random_bytes(unsigned char *target, int size);
uint8_t *serialize_server_hello(TLSServerRecord *sHello, int *out_len);

#endif