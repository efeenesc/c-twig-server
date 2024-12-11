#include "tls_types.h"
#include "tls_consts.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

void *read_into_TLS_record(unsigned char *buf) {
  int curr_offset = 0;
  TLSClientRecord *rec = malloc(sizeof(TLSClientRecord));
  rec->content_type = buf[TLS_HELLO_SIGNATURE_OFFSET];

  rec->version = (buf[TLS_HELLO_VERSION_OFFSET] << 8) | buf[TLS_HELLO_VERSION_OFFSET + 1];
  rec->length = (buf[TLS_HELLO_LENGTH_OFFSET] << 8) | buf[TLS_HELLO_LENGTH_OFFSET + 1];
  rec->message_type = buf[TLS_HELLO_TYPE_OFFSET];
  rec->tls_hello = (buf[TLS_HELLO_BYTES_OFFSET] << 16) |
                      (buf[TLS_HELLO_BYTES_OFFSET + 1] << 8) |
                      buf[TLS_HELLO_BYTES_OFFSET + 2];
  rec->tls_version = (buf[TLS_HELLO_ACTUAL_VERSION_OFFSET] << 8) |
                        buf[TLS_HELLO_ACTUAL_VERSION_OFFSET + 1];

  for (int i = 0; i < TLS_HELLO_RANDOM_LENGTH; i++) {
    rec->tls_random[i] = buf[TLS_HELLO_RANDOM_OFFSET + i];
  }

  rec->session_id_length = buf[TLS_HELLO_S_ID_LENGTH_OFFSET];
  if (rec->session_id_length != 0) {
    rec->p_session_id = malloc(rec->session_id_length);
    curr_offset = rec->session_id_length;

    printf("Session ID length: %d %2x\n", rec->session_id_length,
            rec->session_id_length);

    for (int i = 0; i < rec->session_id_length; i++) {
      rec->p_session_id[i] = buf[TLS_HELLO_S_ID_START_OFFSET + i];
    }
  }

  rec->cipher_list_length = (buf[TLS_HELLO_CIPHER_LENGTH_OFFSET + curr_offset] << 8) +
                            buf[TLS_HELLO_CIPHER_LENGTH_OFFSET + curr_offset + 1];

  // Check if the buffer is large enough to contain the cipher list
  size_t required_size =
      TLS_HELLO_CIPHER_LENGTH_OFFSET + curr_offset + rec->cipher_list_length;
  if (required_size > rec->length + 4) {
    printf("Buffer too small for expected cipher list length!\n");
    free(rec);
    return NULL;
  }

  rec->p_cipher_list = malloc(rec->cipher_list_length);

  for (int i = 0, k = TLS_HELLO_CIPHER_LIST_START_OFFSET + curr_offset;
        i < rec->cipher_list_length / 2; i++, k += 2) {
    rec->p_cipher_list[i] = (buf[k] << 8) | buf[k + 1];
  }

  curr_offset += rec->cipher_list_length;
  rec->compression_methods_length =
      (buf[TLS_HELLO_COMPRESSION_METHODS_LENGTH_OFFSET + curr_offset] << 8) |
      buf[TLS_HELLO_COMPRESSION_METHODS_LENGTH_OFFSET + 1 + curr_offset];

  if (rec->compression_methods_length > 0) {
    required_size = TLS_HELLO_COMPRESSION_METHODS_START_OFFSET + curr_offset +
                    rec->compression_methods_length;

    if (required_size > rec->length + 4) {
      printf("Buffer too small for expected compression methods length!\n");
      free(rec);
      return NULL;
    }

    rec->p_compression_methods_list = malloc(rec->compression_methods_length);

    for (int i = 0; i < rec->compression_methods_length; i++) {
      rec->p_compression_methods_list[i] =
          buf[TLS_HELLO_COMPRESSION_METHODS_START_OFFSET + curr_offset + i];
    }

    curr_offset += rec->compression_methods_length;
  }

  rec->extensions_length =
      (buf[TLS_HELLO_EXTENSIONS_LENGTH_OFFSET + curr_offset] << 8) |
      buf[TLS_HELLO_EXTENSIONS_LENGTH_OFFSET + curr_offset + 1];
  printf("Extensions length is %d %4x\n", rec->extensions_length,
          rec->extensions_length);

  // for (int i = 0; i < rec->cipher_list_length / 2; i++) {
  //   printf("%s\n", getCipherSuiteName(rec->p_cipher_list[i]));
  // }

  return rec;
}

const char *getCipherSuiteName(uint16_t value) {
  switch (value) {
  case TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256:
    return "TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256";
  case TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256:
    return "TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256";
  case TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256:
    return "TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256";
  case TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384:
    return "TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384";
  case TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256:
    return "TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256";
  case TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384:
    return "TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384";
  case TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA:
    return "TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA";
  case TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA:
    return "TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA";
  case TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA:
    return "TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA";
  case TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA:
    return "TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA";
  case TLS_RSA_WITH_AES_128_GCM_SHA256:
    return "TLS_RSA_WITH_AES_128_GCM_SHA256";
  case TLS_RSA_WITH_AES_256_GCM_SHA384:
    return "TLS_RSA_WITH_AES_256_GCM_SHA384";
  case TLS_RSA_WITH_AES_128_CBC_SHA:
    return "TLS_RSA_WITH_AES_128_CBC_SHA";
  case TLS_RSA_WITH_AES_256_CBC_SHA:
    return "TLS_RSA_WITH_AES_256_CBC_SHA";
  case TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA:
    return "TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA";
  case TLS_RSA_WITH_3DES_EDE_CBC_SHA:
    return "TLS_RSA_WITH_3DES_EDE_CBC_SHA";
  default:
    printf("Unknown cipher: %4x\n", value);
    return "Unknown cipher suite";
  }
}