#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cassert>
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

	KVStore(size_t nodes) {
		index = nodes;
		kvstore = new Map();
	}

	~KVStore() {
		delete kvstore;
	}

	Value* get_(Key* key) {
		return dynamic_cast<Value*>(kvstore->get(key)); // returns nullptr if key is not in map
	}

	
	DataFrame* get(Key* key) {
		Value* df_v = get_(key);
		assert(df_v);
		Deserializer des;
		DataFrame* df = df_v->deserialize(des);
		return df;
	}

	void put(Key* key, Value* value) {
		kvstore->set(key, value);
	}

	DataFrame* waitAndGet(Key* key) {
		Value* df_v = get_(key);
		Deserializer des;

		if (df_v) {
			DataFrame* df = df_v->deserialize(des);
			return df;
		}
		else {
			// request get from other kvstores
			while (!df_v) {
				df_v = get_(key);
			}	
			return df_v->deserialize(des);
		}
	}
};


