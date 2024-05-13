#include "http2_server.h"



int main(int argc, char **argv) {
  struct sigaction act;

  if (argc < 4) {
    fprintf(stderr, "Usage: libevent-server PORT KEY_FILE CERT_FILE\n");
    exit(EXIT_FAILURE);
  }

  memset(&act, 0, sizeof(struct sigaction));
  act.sa_handler = SIG_IGN;
  sigaction(SIGPIPE, &act, NULL);

  run(argv[1], argv[2], argv[3]);
  return 0;
}