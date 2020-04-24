
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

	KVStore(size_t idx, char *ip, size_t total_nodes)
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
		cv.wait(cv_mtx);
		cv_mtx.unlock();
		if (kv->check_key_exists(key))
		{
			Value *df_v = get_(key);
			assert(df_v);
			Deserializer *des = new Deserializer(df_v->data_);
			DataFrame *df = DataFrame::deserialize(des);
			return df;
		}
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
		if (idx == index)
		{
			kv->set(key, value);
			size_t len = node->pending_messages->length();
			if (len > 0)
			{
				for (size_t i = 0; i < len; i++)
				{
					Message *m = dynamic_cast<Message *>(node->pending_messages->get_(i));
					if (m->getKind() == MsgKind::KeyMessage)
					{
						KeyMessage *d = dynamic_cast<KeyMessage *>(m);
						Key *v = d->key_;
						assert(v);
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

		// Message *recvd = node->recv_m();
		// if (idx == index || recvd->getKind() == MsgKind::Put) {
		// 	kv->set(key, value);
		// }
		else
		{
		}
		cv.notify_all();
	}

	DataFrame *waitAndGet(Key *key)
	{
		size_t idx = key->home_node_;
		Value *df_v = get_(key);

		DataFrame *df;
		if (key->home_node_ == index)
		{
			df = get(key);
			int i = 0;
			while (i < 500)
			{
				if (Message *sent = node->recv_m())
				{
					if (sent->getKind() == MsgKind::WaitAndGet)
					{
						Data *data_m = new Data(MsgKind::Data, index, sent->getSender(), 11037, df_v);
						node->send_m(data_m);
					}
				}
				i += 1;
			}
			return df;
		}
		else
		{
			KeyMessage *da = new KeyMessage(MsgKind::KeyMessage, index, idx, 2, key);
			node->send_m(da);
			std::cout << "I just printed a key! :)\n";

			while (!df_v)
			{
				Message *val = node->recv_m();
				assert(val->kind_ == MsgKind::Data);
				Data *data = dynamic_cast<Data *>(val);
				if (val)
				{
					put(key, data->v_);
				}
				df_v = get_(key);
			}
			Deserializer *des = new Deserializer(df_v->data_);
			df = new DataFrame(des);
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
	FloatColumn *fc = new FloatColumn();
	fc->push_back(val);
	df->add_column(fc, key.key);
	Serializer *serializer = new Serializer();
	df->serialize(serializer);
	Value *df_v = new Value(serializer->getSerChar());
	kv.put(&key, df_v);
	return df;
}
