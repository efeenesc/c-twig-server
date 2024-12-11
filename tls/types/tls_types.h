#ifndef tls_types_h
#define tls_types_h

#include <stdint.h>
#include "tls_consts.h"

typedef struct __attribute__((packed)) {
	uint8_t content_type;  // 0x16 for handshake record
	uint16_t version;
	uint16_t length;

	uint8_t message_type; // 0x01 for client, 0x02 for server
	uint32_t tls_hello;
	
	uint16_t tls_version;
	unsigned char tls_random[TLS_HELLO_RANDOM_LENGTH];

	uint8_t session_id_length;
	unsigned char *p_session_id;
	
	uint16_t cipher_list_length;
	uint16_t *p_cipher_list;

	uint8_t compression_methods_length;
	uint8_t *p_compression_methods_list;

	uint16_t extensions_length;
} TLSClientRecord;

typedef struct __attribute__((packed)) {
	uint8_t content_type;  // 0x16 for handshake record
	uint16_t version;
	uint16_t length;

	uint8_t message_type; // 0x01 for client, 0x02 for server
	uint8_t pad_byte; // 0x00, this byte is skipped
	uint16_t tls_hello;
	
	uint16_t tls_version;
	unsigned char tls_random[TLS_HELLO_RANDOM_LENGTH];

	uint8_t session_id_length;
	unsigned char *p_session_id;
	
	uint16_t selected_cipher;
	uint8_t compression_methods_length;

	uint16_t extensions_length;
	uint8_t *p_extensions_array;
} TLSServerRecord;

typedef enum {
	TLS_30 = 0x0300,
	TLS_31 = 0x0301,
	TLS_32 = 0x0302,
	TLS_33 = 0x0303
} TLSVersion;

typedef enum {
    TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256 = 0xCCA8,
    TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256 = 0xCCA9,
    TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256 = 0xC02F,
    TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384 = 0xC030,
    TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256 = 0xC02B,
    TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384 = 0xC02C,
    TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA = 0xC013,
    TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA = 0xC009,
    TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA = 0xC014,
    TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA = 0xC00A,
    TLS_RSA_WITH_AES_128_GCM_SHA256 = 0x009C,
    TLS_RSA_WITH_AES_256_GCM_SHA384 = 0x009D,
    TLS_RSA_WITH_AES_128_CBC_SHA = 0x002F,
    TLS_RSA_WITH_AES_256_CBC_SHA = 0x0035,
    TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA = 0xC012,
    TLS_RSA_WITH_3DES_EDE_CBC_SHA = 0x000A
} TLSCipherSuite;

void* read_into_TLS_record(unsigned char *buf);
const char* getCipherSuiteName(uint16_t value);

#endif