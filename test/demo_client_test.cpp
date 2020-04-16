#include <stdio.h>
#include <stdlib.h>
#include "../src/app/demo.h"

int main(int argc, char** argv) {
  //./main server-address server-port client-address client-port idx
  char* server_address = argv[1];
  int server_port = atoi(argv[2]);
  char* client_address = argv[3];
  int client_port = atoi(argv[4]);
  int idx = atoi(argv[5]);
  Demo* demo = new Demo(idx, client_address, 3);
  demo->kv->node->client_init(idx, client_port, server_address, server_port); 
  demo->run_();
  return 0;
}
