#include "server.h"

#include <psdk_inc/_socket_types.h>
#include <stdio.h>
#include <sys/time.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "utils/hashmap.h"

#include "http.h"

#define DEFAULT_BUFLEN 8192

int start(char *port) {
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

    ClientSocket = accept(*socket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
      printf("Invalid client socket\n");
      break;
    } else {
      handleClientConnection(&ClientSocket, DEFAULT_BUFLEN);
    }
  }

  return 0;
}

void handleClientConnection(SOCKET *ClientSocket, UINT16 buflen) {
  // Create buffer char array. This array will be filled by recv
  char recvbuf[buflen];
  int iResult, iSendResult;
  int recvbuflen = buflen;

  ZeroMemory(&recvbuf, recvbuflen);

  // Receive until the peer shuts down the connection
  do {
    iResult = recv(*ClientSocket, recvbuf, recvbuflen, 0);
    if (iResult > 0) {
      // printf("Bytes received: %d\n", iResult);
      // printf("Received:\n%s\n", recvbuf);
      void *response = NULL;
      unsigned long long responseSize = 0;
      int result = handle_http(recvbuf, &response, &responseSize);

      // iSendResult = send(*ClientSocket, recvbuf, iResult, 0);
      iSendResult = send(*ClientSocket, response, responseSize, 0);
      if (iSendResult == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        closeSocket(ClientSocket);
        // return 1;
      }
      // printf("Bytes sent: %d\n", iSendResult);
    } else if (iResult == 0) {
      printf("Connection closing...\n");
    } else {
      printf("recv failed: %d\n", WSAGetLastError());
      closeSocket(ClientSocket);
      // return 1;
    }

  } while (iResult > 0);
}

void closeSocket(SOCKET *socket) {
  closesocket(*socket);
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