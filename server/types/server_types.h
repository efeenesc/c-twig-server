#ifndef server_types_h
#define server_types_h

#include <winsock2.h>
#include <psdk_inc/_socket_types.h>

typedef struct {
    u_short SocketId;
    SOCKET ClientSocket;
} CUSTOMSOCKET;

#endif