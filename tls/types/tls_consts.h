#ifndef tls_consts_h
#define tls_consts_h

// ? Client and server hello types
#define TLS_HELLO_SIGNATURE_OFFSET 0 // 1 byte
#define TLS_HELLO_VERSION_OFFSET 1 // 2 bytes
#define TLS_HELLO_LENGTH_OFFSET 3 // 2 bytes

#define TLS_HELLO_TYPE_OFFSET 5 // 1 byte
#define TLS_HELLO_BYTES_OFFSET 6 // 3 bytes

#define TLS_HELLO_ACTUAL_VERSION_OFFSET 9 // 2 bytes
#define TLS_HELLO_RANDOM_OFFSET 11 // 32 bytes
#define TLS_HELLO_RANDOM_LENGTH 32

#define TLS_HELLO_S_ID_LENGTH_OFFSET 43 // 1 byte
#define TLS_HELLO_S_ID_START_OFFSET 44 // length determined by LENGTH_OFFSET

// Following TLS_HELLO_S_ID_START_OFFSET, add the session ID length to the offsets below
#define TLS_HELLO_CIPHER_LENGTH_OFFSET 44 // 2 bytes
#define TLS_HELLO_CIPHER_LIST_START_OFFSET 46 // length of TLS_HELLO_CIPHER_LENGTH_OFFSET

// Add the TLS cipher list length as well as the session ID length to offsets from this point on
#define TLS_HELLO_COMPRESSION_METHODS_LENGTH_OFFSET 47 // 1 byte
#define TLS_HELLO_COMPRESSION_METHODS_START_OFFSET 48 // length determined by METHODS_LENGTH_OFFSET

// Add the compression methods length here as well.
// Compression methods might not be specified with the methods length set to 0.
// This is why EXTENSIONS_LENGTH OFFSET == COMPRESSION_METHODS_START_OFFSET
#define TLS_HELLO_EXTENSIONS_LENGTH_OFFSET 48 // 2 bytes
#define TLS_HELLO_EXTENSIONS_START 50 // length determined by EXTENSIONS_LENGTH_OFFSET

#endif