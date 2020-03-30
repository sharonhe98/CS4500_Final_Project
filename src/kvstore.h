#pragma once
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

class Value : public Object {
	public:
		char* data_;
		size_t len_;

		Value(char* data) {
			data_ = data;
			len_ = strlen(data);
		}
};

class KVStore : public Object {
	public:
		size_t index;
		Map* kvstore;
		// Array of keys?

	KVStore(size_t nodes) {
		index = nodes;
		kvstore = new Map();
	}

	~KVStore() {
		delete kvstore;
	}

	
	DataFrame* get(Key key) {
		
		return; //ds.readDataFrame();
	}

	void put(Key* key, Value* value) {
		kvstore->set(key, value);
	}

	DataFrame* waitAndGet(Key* key) {
		// somehow wait for all threads to finish
		// when threads are finished, get value
		// create DataFrame after values are gotten
		// return the dataframe
	}
};


