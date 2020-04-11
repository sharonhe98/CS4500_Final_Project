#include "../src/kvstore/kvstore.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	KVStore* kv = new KVStore(1, argv[3], 2);
	size_t idx = atoi(argv[5]);

	std::cout << idx << std::endl;

	size_t my_port = atoi(argv[4]);
	size_t server_port = atoi(argv[2]);
	printf("%zu, %zu, %s, %zu\n", idx, my_port, argv[1], server_port);
	kv->node->client_init(1, my_port, argv[1], server_port);
	return 0;
}
