#include <winsock2.h>

#ifndef server_h
#define server_h

#endif

int start(char port[]);
int listenForConnections(SOCKET *socket);
void closeSocket(SOCKET *socket);
void handleClientConnection(SOCKET *ClientSocket, UINT16 buflen);
int getTime();