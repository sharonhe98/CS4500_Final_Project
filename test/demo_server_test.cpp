#include <stdio.h>
#include <stdlib.h>
#include "../src/app/demo.h"
#include <thread>

int main(int argc, char** argv) {
  //./main server-address server-port idx
  char* server_address = argv[1];
  int server_port = atoi(argv[2]);
  int idx = atoi(argv[3]);
  Demo* demo = new Demo(idx, server_address, 3);
  //std::thread network(&Demo::kv->node->server_init, demo, idx, server_port); 
  //std::thread app(&Demo::run_, demo);
  demo->kv->node->server_init(idx, server_port);
  demo->run_();
  return 0;
}
