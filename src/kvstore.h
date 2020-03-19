#include <stdlib.h>
#include <stdio.h>
#include "object.h"

class Key : public Object {
	public:
		String* key;
		size_t home_node_;

	Key(const char* search_key, node_number) {
		key = new String(search_key);
		home_node_ = node_number;
	}

}

class Value {}

class KVStore : public Object {
	public:
		Key** keys_;
		Value** values_;

	// TODO: figure out constructor
	
	Value* get(Key* key) {
		return value_;
	}
}
