#include "tls.h"
#include "types/tls_types.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Macro to invert bytes from little endian to big endian for networking
#define NETWORK_COPY_TO_BUFFER(src, len)                                               \
  do {                                                                         \
    if (sizeof(*(src)) == sizeof(uint16_t)) {                                  \
      uint16_t value = *(src);                                                 \
      uint8_t high_byte = (value >> 8) & 0xFF;                                 \
      uint8_t low_byte = value & 0xFF;                                         \
      memcpy(buffer + offset, &high_byte, sizeof(uint8_t));                    \
      offset += sizeof(uint8_t);                                               \
      memcpy(buffer + offset, &low_byte, sizeof(uint8_t));                     \
      offset += sizeof(uint8_t);                                               \
    } else {                                                                   \
      memcpy(buffer + offset, (src), (len));                                   \
      offset += (len);                                                         \
    }                                                                          \
  } while (0)

int initiate_tls_call(char *buf, int len, SOCKET sock) {
  unsigned char *ubuf = (unsigned char *)buf;
  // for (int i = 0; i < len; i++) {
  //   printf(" %2x |", ubuf[i]);
  // }
  TLSClientRecord *cHello = read_into_TLS_record(ubuf);

  if (cHello == NULL) {
    printf("Invalid TLS record!\n");
    return 1;
  }

  TLSServerRecord *sHello = create_server_hello(cHello);
  int sHelloSize = 0;

  uint8_t *sHelloSerial = serialize_server_hello(sHello, &sHelloSize);

  for (int i = 0; i < sHelloSize; i++) {
    printf(" %2x |", sHelloSerial[i]);
  }

  send(sock, (const char *)sHelloSerial, sHelloSize, 0);



  return 0;
}

void *create_server_hello(TLSClientRecord *cHello) {
  TLSServerRecord *sHello = malloc(sizeof(TLSServerRecord));

  sHello->content_type = 0x16; // handshake record
  sHello->version = 0x0303;    // TLS1.2
  sHello->message_type = 0x02; // server hello type
  sHello->tls_version = 0x0303;

  // fill tls_random with 32 bytes of random chars
  srand(237592);
  generate_random_bytes(sHello->tls_random, 32);

  sHello->session_id_length = 32;
  sHello->p_session_id = malloc(32);
  generate_random_bytes(sHello->p_session_id, 32);

  sHello->selected_cipher = 0xc013;
  sHello->compression_methods_length = 0x00;

  sHello->extensions_length = 0x0005;
  sHello->p_extensions_array = malloc(5 * sizeof(uint8_t));
  if (!sHello->p_extensions_array) {
    free(sHello->p_session_id);
    free(sHello);
    return NULL;
  }
  sHello->p_extensions_array[0] = 0xff;
  sHello->p_extensions_array[1] = 0x01;
  sHello->p_extensions_array[2] = 0x00;
  sHello->p_extensions_array[3] = 0x01;
  sHello->p_extensions_array[4] = 0x00;

  sHello->tls_hello = sizeof(sHello->tls_version) + sizeof(sHello->tls_random) +

                      sizeof(sHello->session_id_length) +
                      sHello->session_id_length +

                      sizeof(sHello->selected_cipher) +
                      sizeof(sHello->compression_methods_length) +

                      sizeof(sHello->extensions_length) +
                      sHello->extensions_length; // dynamic length

  sHello->length = sizeof(sHello->message_type) + sizeof(sHello->pad_byte) +
                    sizeof(sHello->tls_hello) + sHello->tls_hello;

  return sHello;
}

uint8_t *serialize_server_hello(TLSServerRecord *sHello, int *out_len) {
  uint8_t *buffer = malloc(sHello->length + 5);
  if (!buffer)
    return NULL;

  size_t offset = 0;
  unsigned int empty = 0;

  NETWORK_COPY_TO_BUFFER(&sHello->content_type, sizeof(sHello->content_type));
  NETWORK_COPY_TO_BUFFER(&sHello->version, sizeof(sHello->version));
  NETWORK_COPY_TO_BUFFER(&sHello->length, sizeof(sHello->length));

  NETWORK_COPY_TO_BUFFER(&sHello->message_type, sizeof(sHello->message_type));
  NETWORK_COPY_TO_BUFFER(&empty, sizeof(uint8_t));
  NETWORK_COPY_TO_BUFFER(&sHello->tls_hello, sizeof(sHello->tls_hello));
  NETWORK_COPY_TO_BUFFER(&sHello->tls_version, sizeof(sHello->tls_version));
  NETWORK_COPY_TO_BUFFER(sHello->tls_random, sizeof(sHello->tls_random));

  NETWORK_COPY_TO_BUFFER(&sHello->session_id_length, sizeof(sHello->session_id_length));
  NETWORK_COPY_TO_BUFFER(sHello->p_session_id, sHello->session_id_length);

  NETWORK_COPY_TO_BUFFER(&sHello->selected_cipher, sizeof(sHello->selected_cipher));
  NETWORK_COPY_TO_BUFFER(&sHello->compression_methods_length,
                 sizeof(sHello->compression_methods_length));

  NETWORK_COPY_TO_BUFFER(&sHello->extensions_length, sizeof(sHello->extensions_length));
  NETWORK_COPY_TO_BUFFER(sHello->p_extensions_array, sHello->extensions_length);

  // Set the output length
  *out_len = sHello->length + 5;

  return buffer;
}

void generate_random_bytes(unsigned char *target, int size) {
  for (int i = 0; i < size; i++) {
    int value = rand() % 256;
    target[i] = value;
  }
}

unsigned char* generate_server_cert_message() {
  
}