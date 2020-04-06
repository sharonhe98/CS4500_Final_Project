#include "../src/kvstore.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

int main(int argc, char** argv) {
	size_t idx = atoi(argv[3]);
	size_t port = atoi(argv[2]);
	KVStore* kv = new KVStore(0, argv[1], 2);
	kv->node->server_init(idx, port);
	return 0;
}
