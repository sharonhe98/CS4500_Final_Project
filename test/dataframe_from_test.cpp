#include <stdio.h>
#include <stdlib.h>
#include <cassert>
#include "../src/kvstore/kvstore.h"

void test_from_array() {
	size_t SZ = 100;
	double* fa = new double[SZ];
	for (size_t i = 0; i < SZ; i++) {
		fa[i] = 5.0;
	}
	Key * k = new Key("fa", 0);
	KVStore* kv = new KVStore(0, "127.0.0.1", 1);
	DataFrame* df = fromArray(*k, *kv, SZ, fa);
	assert(df->get_double(0, 30) == 5.0);
	delete k;
	delete kv;
	delete [] fa;
	printf("test fromArray success!\n");
}

void test_from_scalar() {
	double sca = 500.0;
	Key * k = new Key("f", 0);
	KVStore* kv = new KVStore(0, "127.0.0.1", 1);
	DataFrame* df = fromScalar(*k, *kv, sca);
	assert(df->get_double(0,0) == 500.0);
	delete k;
	delete kv;
	printf("test fromScalar success!\n");
}

int main() {
	test_from_array();
	test_from_scalar();
	return 0;
}
