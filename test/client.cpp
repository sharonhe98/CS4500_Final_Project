#include "../src/kvstore.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	KVStore* kv = new KVStore(2, argv[3]);
	size_t idx = atoi(argv[5]);
	size_t my_port = atoi(argv[4]);
	size_t server_port = atoi(argv[2]);
	printf("%zu, %zu, %s, %zu\n", idx, my_port, argv[1], server_port);
	kv->node->client_init(idx, my_port, argv[1], server_port);
	return 0;
}
