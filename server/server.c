#include "server.h"

#include <minwinbase.h>
#include <minwindef.h>
#include <psdk_inc/_socket_types.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/time.h>
#include <winsock.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "../http/http.h"
#include "types/server_types.h"

#define DEFAULT_BUFLEN 8192
#define MAX_CONCURRENT_SOCKETS USHRT_MAX
CUSTOMSOCKET g_Sockets[MAX_CONCURRENT_SOCKETS];

// g_SocketNum is meant to be overflowable. This is a way to loop through g_Sockets
u_short g_SocketNum = 0;

int start(char *port) {
  // Initialize g_Sockets to 0
  memset(&g_Sockets, 0, sizeof(g_Sockets));

  WSADATA wsaData;

  int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (iResult != 0) {
    printf("Failed to start WSA\n");
    return 1;
  }

  struct addrinfo *result = NULL, *ptr = NULL, hints;

  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;

  // Resolve the local address and port to be used by the server
  iResult = getaddrinfo("localhost", port, &hints, &result);
  if (iResult != 0) {
    printf("getaddrinfo failed: %d\n", iResult);
    WSACleanup();
    return 1;
  }

  SOCKET ListenSocket = INVALID_SOCKET;

  ListenSocket =
      socket(result->ai_family, result->ai_socktype, result->ai_protocol);

  if (ListenSocket == INVALID_SOCKET) {
    printf("Failed to create socket! %d\n", WSAGetLastError());
    WSACleanup();
    return 1;
  }

  iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
  if (iResult != 0) {
    printf("Failed to bind socket! %d\n", WSAGetLastError());
    FreeAddrInfo(result);
    closesocket(ListenSocket);
    WSACleanup();
    return 1;
  } else {
    printf("Server started on port %s\n", port);
  }

  FreeAddrInfo(result);
  listenForConnections(&ListenSocket);
  return 0;
}

int listenForConnections(SOCKET *socket) {
  int iResult = listen(*socket, SOMAXCONN);
  if (iResult == SOCKET_ERROR) {
    printf("Failed to start listening on socket! %d", WSAGetLastError());
    closesocket(*socket);
    WSACleanup();
    return 1;
  }

  for (;;) {
    SOCKET ClientSocket = INVALID_SOCKET;

    // Blocking call. 'accept' returns a socket mem address
    ClientSocket = accept(*socket, NULL, NULL);

    if (ClientSocket == INVALID_SOCKET) {
      printf("Invalid client socket\n");
      break;
    }

    u_short thisSocketNum;

    // Run through the g_Sockets array until a free client socket is found
    do {
      thisSocketNum = g_SocketNum;
      g_SocketNum++;
    } while (g_Sockets[thisSocketNum].ClientSocket != 0);

    // Copy the client socket address into the free custom socket struct, assign the ID as well 
    memcpy(&g_Sockets[thisSocketNum].ClientSocket, &ClientSocket, sizeof(ClientSocket));
    g_Sockets[thisSocketNum].SocketId = thisSocketNum; 

    // Create a thread to handle the rest of the process and continue listening for new client connections 
    pthread_t server_thread;
    void *handle_connection = &handleClientConnection;
    pthread_create(&server_thread, NULL, handle_connection, &g_Sockets[thisSocketNum]);
    printf("Passing to thread...");
  }

  return 0;
}

void handleClientConnection(CUSTOMSOCKET *socket) {
  // Create buffer char array. This array will be filled by recv
  char recvbuf[DEFAULT_BUFLEN];
  int iResult, iSendResult;
  int recvbuflen = DEFAULT_BUFLEN;

  ZeroMemory(&recvbuf, recvbuflen);

  // Receive request payload from client, process in handle_http, get the response, send it, free socket
  do {
    iResult = recv(socket->ClientSocket, recvbuf, recvbuflen, 0);
    if (iResult > 0) {
      // printf("Bytes received: %d\n", iResult);
      // printf("Received:\n%s\n", recvbuf);
      void *response = NULL;
      unsigned long long responseSize = 0;
      int result = handle_http(recvbuf, &response, &responseSize);
      
      iSendResult = send(socket->ClientSocket, response, responseSize, 0);
      if (iSendResult == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        closeCustomSocket(socket);
        // return 1;
      }
      // printf("Bytes sent: %d\n", iSendResult);
    } else if (iResult == 0) {
      printf("Connection closing...\n");
    } else {
      printf("recv failed: %d\n", WSAGetLastError());
      closeCustomSocket(socket);
    }

  } while (iResult > 0);
}

void closeSocket(SOCKET *socket) {
  closesocket(*socket);
  WSACleanup();
}

void closeCustomSocket(CUSTOMSOCKET *socket) {
  closesocket(socket->ClientSocket);
  u_short sockId = socket->SocketId;
  free(socket);
  g_Sockets[sockId].ClientSocket = INVALID_SOCKET;
  g_Sockets[sockId].SocketId = 0;
  WSACleanup();
}

int getTime() {
  struct timeval te;
  gettimeofday(&te, NULL); // get current time
  long long milliseconds =
      te.tv_sec * 1000LL + te.tv_usec / 1000; // calculate milliseconds
  // printf("milliseconds: %lld\n", milliseconds);
  return milliseconds;
}