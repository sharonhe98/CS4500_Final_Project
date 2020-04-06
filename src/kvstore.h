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
	NetworkIP* node;

	KVStore(size_t idx, char* ip, size_t total_nodes)
	{
		index = idx;
		kv = new Map();
		node = new NetworkIP(ip, total_nodes, idx);
	}

	~KVStore()
	{
		delete node;
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
		Deserializer *des = new Deserializer(df_v->data_);
		DataFrame* df = new DataFrame(des);
		return df;
	}

	void put(Key *key, Value *value)
	{
		size_t idx = key->home_node_;
		if (idx == index || node->recv_m()->getKind() == MsgKind::Put) { kv->set(key, value); }
		Put* msg = new Put(MsgKind::Put, index, idx, index);
		node->send_m(msg);
	}

	DataFrame *waitAndGet(Key *key)
	{
		Value *df_v = get_(key);
		DataFrame *df;
		Message* sent = node->recv_m();
		if (key->home_node_ == index)
		{
			df = get(key);
		}
		else if (sent->getKind() == MsgKind::WaitAndGet) {
			Message* m = new Message(MsgKind::Status, index, sent->getTarget(), 500);
			node->send_m(m);
		}
		else
		{
			// request get from other kvstores
			while (!df_v)
			{
				df_v = get_(key);
			}
			Deserializer *des = new Deserializer(df_v->data_);
			df = new DataFrame(des);
		}
		
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
