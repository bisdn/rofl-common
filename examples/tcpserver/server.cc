#include "tcpserver.h"

int main(int argc, char **argv) {
  rofl::examples::tcpserver server;

  return server.run(argc, argv);
}
