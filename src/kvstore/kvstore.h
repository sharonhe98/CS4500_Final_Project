
#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cassert>
#include "../string.h"
#include "../object.h"
#include "../array.h"
#include "../dataframe/sorer.h"
#include "../map.h"
#include "../network/network.h"
#include <thread>
#include <mutex>
#include <condition_variable>

class KVStore : public Object
{
public:
	size_t index;
	Map *kv;
	NetworkIP *node;
	std::condition_variable_any cv;
	std::mutex cv_mtx;

	KVStore(size_t idx, const char *ip, size_t total_nodes)
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
		// wait for value to get put
		cv.wait(cv_mtx);
		cv_mtx.unlock();
		// if key exists locally, then return df
		if (kv->check_key_exists(key))
		{
			Value *df_v = get_(key);
			assert(df_v);
			Deserializer *des = new Deserializer(df_v->data_);
			DataFrame *df = DataFrame::deserialize(des);
			return df;
		}
		// TODO: send get message to node the key exists on and wait for node to return df
		else
		{
			size_t target = key->home_node_;
			Get *m = new Get(MsgKind::Get, index, target, 53);
			node->send_m(m);
			return nullptr;
		}
	}

	void put(Key *key, Value *value)
	{
		size_t idx = key->home_node_;
		// if in home node
		if (idx == index)
		{
			// put key and value in the store
			kv->set(key, value);
			// check for pending waitandget messages
			size_t len = node->pending_messages->length();
			if (len > 0)
			{
				for (size_t i = 0; i < len; i++)
				{
					Message *m = dynamic_cast<Message *>(node->pending_messages->get_(i));
					// if the message has key
					if (m->getKind() == MsgKind::KeyMessage)
					{
						KeyMessage *d = dynamic_cast<KeyMessage *>(m);
						Key *v = d->key_;
						assert(v);
						// if keys match send value back over network
						if (strcmp(key->key->c_str(), v->key->c_str()) == 0)
						{
							Put *msg = new Put(MsgKind::Put, index, idx, index);
							node->send_m(msg);

							Data *response = new Data(MsgKind::Data, index, m->getSender(), m->id_, value);
							node->send_m(response);
						}
					}
				}
			}
		}
		else
		{
			kv->set(key, value);
		}
		// notify all nodes that value has been put
		cv.notify_all();
	}

	DataFrame *waitAndGet(Key *key)
	{
		size_t idx = key->home_node_;
		Value *df_v = get_(key);

		DataFrame *df;
		// if key node equals node index
		if (key->home_node_ == index)
		{
			// handles the waiting and return
			df = get(key);
			return df;
		}
		else
		{
			// send key that we want value of within a message
			KeyMessage *da = new KeyMessage(MsgKind::KeyMessage, index, idx, 2, key);
			node->send_m(da);
			
			// wait for response
			while (!df_v)
			{
				// get the response
				Message *val = node->recv_m();
				assert(val->kind_ == MsgKind::Data);
				// cast message to data to get the value
				Data *data = dynamic_cast<Data *>(val);
				if (data)
				{
					// put the value in the kv store
					kv->set(key, data->v_);
				}
				df_v = get_(key);
			}
			// deserialize the dataframe
			Deserializer *des = new Deserializer(df_v->data_);
			df = DataFrame::deserialize(des);
			return df;
		}
	}
};

DataFrame *fromArray(Key &key, KVStore &kv, size_t SZ, double *vals)
{
	Schema s("F");
	DataFrame *df = new DataFrame(s);
	FloatColumn *fc = new FloatColumn();
	for (size_t i = 0; i < SZ; i++)
	{
		fc->push_back(vals[i]);
	}
	df->cols[0] = fc;
	Serializer *serializer = new Serializer();
	df->serialize(serializer);
	Value *df_v = new Value(serializer->getSerChar());
	kv.put(&key, df_v);
	return df;
}

// basic implementation of fromScalar using only doubles
DataFrame *fromScalar(Key &key, KVStore &kv, double val)
{
	Schema s("F");
	DataFrame *df = new DataFrame(s);
	df->cols[0]->push_back(val);
	Serializer *serializer = new Serializer();
	df->serialize(serializer);
	Value *df_v = new Value(serializer->getSerChar());
	kv.put(&key, df_v);
	return df;
}
