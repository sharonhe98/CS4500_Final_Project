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

class KVStore : public Object {
	public:
		size_t num_nodes;
		Array* keys_;
		StringArray* values_;

	KVStore(size_t nodes) {
		num_nodes = nodes;
		
	}

	// TODO: figure out constructor
	
	String* get(Key* key) {
		for (size_t i = 0; i < keys_->length(); i++) {
			if (keys->get(i)->equals(key)) {
				return value_;
			}
		}
		return nullptr;
	}

	void set(Key* key, String* value) {
		if (keys_->index_of_(key) > keys_->length()) {
			keys_->append(key);
			values->push_back(value);
		}
		else {
			for (size_t i = 0; i < keys_->length(); i++) {
				if (keys->get(i)->equals(key)) {
					values->set(i, value_);
				}
			}
		}
	}

	String* getAndWait(Key* k) {
		return get(k);
	}
}
