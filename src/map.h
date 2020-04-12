// A1: Part 2
// lang: CwC
#pragma once

#include "object.h"
#include "array.h"

/**
 * A class that represents a map of key value pairs where
 * Key is an Object* that represents the key of a value we want to access and
 * Value is the Object* we want to access
 */
class Map : public Object
{
	// We use Array* to represent keys and values to update them
	// in parallel and keep keys and values in synch
	// We find this easier due to the fact that we have previously implemented Arrays
	// size_ is the number of key-value pairs stored in the map
public:
	Array *keys_;
	Array *values_;
	size_t size_;

	Map()
	{
		// Constructor for Map
		// Initialize keys_ and values_ to new, empty arrays
		size_ = 0;
		keys_ = new Array();
		values_ = new Array();
	}

	~Map()
	{
		// Destructor for Map
		delete keys_;
		delete values_;
	}

	// Returns the amount of entries in this map
	size_t length()
	{
		return size_;
	}

	// check if key exists in map
	bool check_key_exists(Object *key)
	{
		return keys_->index_of(key) < keys_->length();
	}

	// Gets the value at a specific key
	Object *get(Object *key)
	{
		// First find the index of the key we want in our keys_ array
		size_t index = keys_->index_of(key);
		// if the key does not exist, return nullptr
		if (size_ == 0 || index > size_)
		{
			return nullptr;
		}
		// else return the corresponding value
		return values_->get_(index);
	}

	// Sets the value at the specified key to the value
	void set(Object *key, Object *value)
	{
		if (key == nullptr)
		{
			// don't set anything if key is null
			return;
		}
		// check if the key already exists
		size_t index = keys_->index_of(key);
		// if the key doesn't exist, add a new key-value pair
		if (size_ == 0 || index > size_)
		{
			keys_->append(key);
			values_->append(value);
			size_ += 1;
		}
		else
		{
			// if the key exists already, set the value at
			// that key to the desired value
			values_->set(index, value);
		}
	}

	// Gets all the keys of this map
	Object **getKeys()
	{
		return keys_->elements_;
	}

	// Gets all the values of this map
	Object **getValues()
	{
		return values_->elements_;
	}

	// Checks if this map is equal to another object
	bool equals(Object *o)
	{
		// check for nullptr and length differences
		// o cannot equal this map if it is nullptr
		// (by default since this is not null)
		// and if the map sizes are different, the key-value pairs are also unequal
		if (o == nullptr)
		{
			return false;
		}
		Map *m = dynamic_cast<Map *>(o);
		if (m == nullptr)
		{
			return false;
		}
		if (size_ != m->length())
		{
			return false;
		}
		else
		{
			for (size_t i = 0; i < keys_->length(); i++)
			{
				if (m->get(keys_->get_(i)) == nullptr)
					return false;
				if (get(keys_->get_(i))->equals(m->get(keys_->get_(i))))
					return true;
			}
			return false;
		}
	}

	// Returns the hash of this map
	size_t hash()
	{
		return keys_->hash();
	};
};

class StringMap : public Map
{
public:
	StringMap() {}

	String *get(String *k) { return dynamic_cast<String *>(Map::get(k)); }
};
