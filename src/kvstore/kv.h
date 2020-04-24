#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cassert>
#include "../string.h"
#include "../object.h"

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

	Key(Deserializer *d)
	{
		key = d->readString();
		home_node_ = d->readSizeT();
	}

	void serialize(Serializer *s)
	{
		s->write(key);
		s->write(home_node_);
	}

	static Key *deserialize(Deserializer *d)
	{
		return new Key(d);
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

	Value(Deserializer *d)
	{
		data_ = d->readChars();
		len_ = d->readSizeT();
	}

	void serialize(Serializer *s)
	{
		s->write(data_);
		s->write(len_);
	}

	static Value *deserialize(Deserializer *d)
	{
		return new Value(d);
	}
};