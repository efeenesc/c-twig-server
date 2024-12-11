#ifndef server_h
#define server_h

#include "types/server_types.h"
#include <winsock2.h>

int start(char port[]);
int listenForConnections(SOCKET *socket);
void handleClientConnection(CUSTOMSOCKET *socket);
void closeSocket(SOCKET *socket);
void closeCustomSocket(CUSTOMSOCKET *socket);
int getTime();

#endif