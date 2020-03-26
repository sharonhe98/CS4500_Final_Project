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

	// Removes all entries in this map
	void clear()
	{
		// if there is something to clear, we loop through
		// and nullify both keys and values at the same time
		if (keys_->length() > 0)
		{
			for (size_t i = 0; i < size_; i++)
			{
				keys_->set(i, nullptr);
				values_->set(i, nullptr);
			}
		}
		size_ = 0;
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

	// Removes value at the specified key and returns the removed object
	Object *remove(Object *key)
	{
		// if the key doesn't exist, there is no Object to remove
		size_t index = keys_->index_of(key);
		if (size_ == 0 || index > size_)
		{
			return nullptr;
		}
		else
		{
			Object *val = values_->get_(index);
			// if the key exists, remove both the key and value pair
			keys_->remove(index);
			values_->remove(index);
			size_ -= 1;
			return val;
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
			// delegate to the keys and values to check equality
			return keys_->equals(m->keys_) && values_->equals(m->values_);
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
	StringArray *keys_;
	StringArray *values_;
	size_t size_;

	StringMap()
	{
		// Constructor for StringMap
		size_ = 0;
		keys_ = new StringArray();
		values_ = new StringArray();
	}

	~StringMap()
	{
		// Destructor for StringMap
		delete keys_;
		delete values_;
	}

	// Returns the amount of entries in this map
	size_t length()
	{
		return size_;
	}

	// Removes all entries in this map
	void clear()
	{
		// only clear if there's something to clear
		if (keys_->length() > 0)
		{
			for (size_t i = 0; i < size_; i++)
			{
				keys_->set(i, nullptr);
				values_->set(i, nullptr);
			}
		}
		size_ = 0;
	}

	// Gets the value at a specific key
	String *get(String *key)
	{
		// find the key index, return the value if the key exists
		size_t index = keys_->index_of(key);
		if (size_ == 0 || index > size_)
		{
			return nullptr;
		}
		return values_->get(index);
	}

	// Sets the value at the specified key to the value
	void set(String *key, String *value)
	{
		if (key == nullptr)
		{
			// don't set anything if key is null
			return;
		}
		// of tje key doesn't exist, add a new key-value pair
		size_t index = keys_->index_of(key);
		if (size_ == 0 || index > size_)
		{
			keys_->append(key);
			values_->append(value);
			size_ += 1;
		}
		else
		{
			// else set the value at the given key
			values_->set(index, value);
		}
	}

	// Removes value at the specified key and returns the removed object
	String *remove(String *key)
	{
		size_t index = keys_->index_of(key);
		if (size_ == 0 || index > size_)
		{
			// nothing to remove if the key-value pair doesn't exist
			return nullptr;
		}
		else
		{
			String *val = values_->get(index);
			// if key exists, remove key-value pair
			keys_->remove(index);
			values_->remove(index);
			size_ -= 1;
			return val;
		}
	}

	// Gets all the keys of this map
	String **getKeys()
	{
		return keys_->elements_;
	}

	// Gets all the values of this map
	String **getValues()
	{
		return values_->elements_;
	}

	// Checks if this map is equal to another object
	bool equals(Object *o)
	{
		// this map is not null, so if the other object/map is
		// a nullptr, return false
		// else if the length of the maps are unequal, the values are also unequal at some point, so return false then, too
		if (o == nullptr)
		{
			return false;
		}
		StringMap *m = dynamic_cast<StringMap *>(o);
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
			// else let the keys and values check equality with each other
			return keys_->equals(m->keys_) && values_->equals(m->values_);
		}
	}

	// Returns the hash of this map
	size_t hash()
	{
		// let keys handle their own hashing
		return keys_->hash();
	}
};
