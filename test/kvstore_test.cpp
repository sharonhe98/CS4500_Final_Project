#include <stdlib.h>
#include <stdio.h>
#include <cassert>
#include <thread>
#include "../src/kvstore/kvstore.h"

using namespace std;

void testGet()
{
	Schema s("II");
	DataFrame *df = new DataFrame(s);
	KVStore *kv = new KVStore(0, "127.0.0.1", 1);
	Key *key = new Key("df_key", 0);
	assert(!kv->get(key));
	Serializer *ser = new Serializer();
	df->serialize(ser);
	char *result = ser->getSerChar();
	Value *v = new Value(result);

	kv->put(key, v);
	assert(kv->get(key)->equals(v));
	printf("kv store get success!\n");
}

void testWaitAndGet()
{
	KVStore *k0 = new KVStore(0, "127.0.0.1", 3);
	KVStore *k1 = new KVStore(1, "127.0.0.1", 3);
	Key *key1 = new Key("df_key", 1);
	Schema s("II");
	DataFrame *df = new DataFrame(s);
	Serializer *ser = new Serializer();
	df->serialize(ser);
	char *result = ser->getSerChar();
	Value v(result);
	k1->put(key1, &v);
	DataFrame *getDF = k0->waitAndGet(key1);
	assert(getDF);
	assert(strcmp(getDF->scm.track_types, "II") == 0);
}

int main()
{
	testGet();
	return 0;
}
