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
	Key k("fa", 0);
	KVStore* kv = new KVStore(0);
	DataFrame* df = fromArray(k, *kv, SZ, fa);
	Serializer* ser = new Serializer();
	df->serialize(ser);
	Value* v = new Value(ser->getSerChar());
	assert(df->get(0, 30) == 5.0);
	assert(df->nrows() == 100);
	assert(kv->get(k)->equals(v));
	printf("test fromArray success!\n");
}

void test_from_scalar() {
	double sca = 500.0;
	Key k("f", 0);
	KVStore* kv = new KVStore(0);
	DataFrame* df = fromScalar(k, *kv, sca);
	Serializer* ser = new Serializer();
	df->serialize(ser);
	Value* v = new Value(ser->getSerChar());
	assert(df->get(0, 0) == 500.0);
	assert(kv->get(k)->equals(v));
	printf("test fromScalar success!\n");
}

int main() {
	test_from_array();
	test_from_scalar();
	return 0;
}
