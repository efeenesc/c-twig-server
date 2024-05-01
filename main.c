#include "main.h"
#include "server.h"
#include <psdk_inc/_socket_types.h>
#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

#define DEFAULT_PORT "23579"

int main(int argc, char *argv[]) {
  pthread_t server_thread;
  void *start_server = &start;
  pthread_create(&server_thread, NULL, start_server, DEFAULT_PORT);
  printf("Working just fine\n");
  pthread_join(server_thread, NULL);
  
  return 0;
}

