#include "main.h"
#include "server.h"

#include "utils/string_util.h"

#include <getopt.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define DEFAULT_PORT "23579"

int main(int argc, char *argv[]) {
  char *port = DEFAULT_PORT;
  bool isCaseInsensitive = false;
  int opt;
  enum { CHARACTER_MODE, WORD_MODE, LINE_MODE } mode = CHARACTER_MODE;

  while ((opt = getopt(argc, argv, "p:")) != -1) {
    switch (opt) {
    case 'p':
      port = optarg;
      if (str_is_number(optarg))
        break;
    default:
      fprintf(stderr, "Usage: %s [-p] [port]\n", argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  pthread_t server_thread;
  void *start_server = &start;
  pthread_create(&server_thread, NULL, start_server, port);
  pthread_join(server_thread, NULL);

  return 0;
}
