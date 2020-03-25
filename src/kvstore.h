#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "string.h"
#include "object.h"
#include "array.h"
#include "dataframe.h"

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
		Array* keys_;
		StringArray* values_;

	KVStore(size_t nodes) {
		num_nodes = nodes;
		
	}

	// TODO: figure out constructor
	
	String* get(Key* key) {
		for (size_t i = 0; i < keys_->length(); i++) {
			if (keys_->get_(i)->equals(key)) {
				return values_->get(i);
			}
		}
		return nullptr;
	}

	void set(Key* key, String* value) {
		if (keys_->index_of(key) > keys_->length()) {
			keys_->append(key);
			values_->append(value);
		}
		else {
			for (size_t i = 0; i < keys_->length(); i++) {
				if (keys_->get_(i)->equals(key)) {
					values_->set(i, value);
				}
			}
		}
	}

	String* getAndWait(Key* k) {
		return get(k);
	}
};

// a class to talk to the application
class KDStore {
	public:
		KVStore* kv_;

	KDStore(KVStore& kv) {
		kv_ = &kv;
	}

	DataFrame* get(Key key) {

	}
};
