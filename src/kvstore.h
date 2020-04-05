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
#include "network.h"

class Key : public Object
{
public:
	String *key;
	size_t home_node_;

	Key(const char *search_key, size_t node_number)
	{
		key = new String(search_key);
		home_node_ = node_number;
	}
};

class Value : public Object
{
public:
	char *data_;
	size_t len_;

	Value(char *data)
	{
		data_ = data;
		len_ = strlen(data);
	}
};

class KVStore : public Object
{
public:
	size_t index;
	Map *kv;

	KVStore(size_t nodes)
	{
		index = nodes;
		kv = new Map();
	}

	~KVStore()
	{
		delete kv;
	}

	Value *get_(Key *key)
	{
		return dynamic_cast<Value *>(kv->get(key)); // returns nullptr if key is not in map
	}

	DataFrame *get(Key *key)
	{
		Value *df_v = get_(key);
		assert(df_v);
		// Deserializer des;
		// DataFrame* df = df_v->deserialize(des);
		// return df;
		DataFrame *df;
		return df;
	}

	void put(Key *key, Value *value)
	{
		kv->set(key, value);
	}

	DataFrame *waitAndGet(Key *key)
	{
		Value *df_v = get_(key);
		// Deserializer des;

		if (df_v)
		{
			// DataFrame* df = df_v->deserialize(des);
			// return df;
		}
		else
		{
			// request get from other kvstores
			while (!df_v)
			{
				df_v = get_(key);
			}
			// return df_v->deserialize(des);
		}
		DataFrame *df;
		return df;
	}
};

DataFrame *fromArray(Key &key, KVStore &kv, size_t SZ, double *vals)
  {
    Schema s("F");
    DataFrame* df = new DataFrame(s);
    FloatColumn* fc = new FloatColumn();
    for (size_t i = 0; i < SZ; i++) {
      fc->push_back(vals[i]);
    }
    df->add_column(fc, key.key);
    Serializer serializer;
    df->serialize(&serializer);
    Value* df_v = new Value(serializer.getSerChar());
    kv.put(&key, df_v);
    return df;
  }

  // basic implementation of fromScalar using only doubles
  DataFrame *fromScalar(Key &key, KVStore &kv, double val)
  {
    Schema s("F");
    DataFrame* df = new DataFrame(s);
    FloatColumn* fc = new FloatColumn();
    fc->push_back(val);
    df->add_column(fc, key.key);
    Serializer serializer;
    df->serialize(&serializer);
    Value* df_v = new Value(serializer.getSerChar());
    kv.put(&key, df_v);
    return df;
  }
