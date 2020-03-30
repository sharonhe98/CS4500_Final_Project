#include <stdlib.h>
#include <stdio.h>
#include <cassert>
#include <thread>
#include "../src/kvstore.h"

using namespace std;

void testGet()
{
	Schema s("II");
	DataFrame *df = new DataFrame(s);
	KVStore *kv = new KVStore(0);
	Key *key = new Key("df_key", 0);
	assert(!kv->get(key));
	Serializer *ser;
	Value *v = new Value(df->serialize(ser));

	kv->put(key, v);
	assert(kv->get(key)->equals(v));
	printf("kv store get success!\n");
}

void testWaitAndGet()
{
	Schema s("II");
	DataFrame *df = new DataFrame(s);
	Key *key = new Key("df_key", 0);
	Serializer *ser;
	Value v(df->serialize(ser));

	KVStore **all_nodes = new KVStore *[3];
	for (size_t i = 0; i < 3; i++)
	{
		all_nodes[i] = new KVStore(i);
	}
	// we don't yet have a network layer, but this would be our expected result
	//	assert(all_nodes[1]->waitAndGet(key)->equals(df));
	//	all_nodes[0]->put(key, v);
}

int main()
{
	testGet();
	return 0;
}
