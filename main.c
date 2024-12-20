#include "main.h"
#include "server/server.h"
#include "files.h"
#include "utils/string_util.h"

#include <getopt.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define DEFAULT_PORT "500"

int main(int argc, char *argv[]) {
  char *port = DEFAULT_PORT;
  bool isCaseInsensitive = false;
  enum { CHARACTER_MODE, WORD_MODE, LINE_MODE } mode = CHARACTER_MODE;
  int opt;

  printf("argv[0]: %s\n", argv[0]);

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

  char *current_cwd = argv[0];
  set_cwd(current_cwd);

  pthread_t server_thread;
  void *start_server = &start;
  pthread_create(&server_thread, NULL, start_server, port);
  pthread_join(server_thread, NULL);

  return 0;
}
