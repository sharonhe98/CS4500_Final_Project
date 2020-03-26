#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "string.h"
#include "object.h"
#include "array.h"
#include "dataframe.h"
#include "map.h"

class Key : public Object {
	public:
		String* key;
		size_t home_node_;

	Key(const char* search_key, size_t node_number) {
		key = new String(search_key);
		home_node_ = node_number;
	}

};

class KVStore : public Object {
	public:
		size_t num_nodes;
		Map* kvstore;

	KVStore(size_t nodes) {
		num_nodes = nodes;
		kvstore = new Map();
	}

	~KVStore() {
		delete kvstore;
	}

	// TODO: figure out constructor
	
	String* get(Key* key) {
		kvstore->get(key);
	}

	void put(Key* key, String* value) {
		kvstore->set(key, value);
	}

	String* getAndWait(Key* k) {
		return get(k);
	}
};


