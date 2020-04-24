#pragma once
#include <cstdlib>
#include <netinet/in.h>
#include "../object.h"
#include "../string.h"
#include "../array.h"
#include "../kvstore/kv.h"

// enum list of message types
enum class MsgKind
{
  Ack,
  Nack,
  Put,

  Reply,
  Get,
  WaitAndGet,
  Status,

  Kill,
  Register,
  Directory,
  Data
};

/**
 * A class that represents a message
 */
class Message : public Object
{
public:
  MsgKind kind_; // the message kind

  size_t sender_; // the index of the sender node

  size_t target_; // the index of the receiver node

  size_t id_; // an id t unique within the node

  // constructor
  Message(MsgKind m, size_t s, size_t t, size_t i)
  {
    kind_ = m;
    sender_ = s;
    target_ = t;
    id_ = i;
  }

  // message constructor that constructs a message after deserializing a message
  Message(Deserializer *d, MsgKind k)
  {
    kind_ = k;
    sender_ = d->readSizeT();
    target_ = d->readSizeT();
    id_ = d->readSizeT();
  }

  // serializes a message
  virtual void serialize(Serializer *ser)
  {
    ser->write((int)kind_);
    ser->write(sender_);
    ser->write(target_);
    ser->write(id_);
  }

  // function declaration
  static Message *deserializeMsg(Deserializer *dser);

  // returns the message kind/type
  MsgKind getKind()
  {
    return kind_;
  }

  // gets the message sender
  size_t getSender()
  {
    return sender_;
  }

  // returns the message target
  size_t getTarget()
  {
    return target_;
  }

  // returns the id
  size_t getId()
  {
    return id_;
  }
};

/**
 * A class that represents an Ack message
 */
class Ack : public Message
{
public:
  // constructor
  Ack(MsgKind m, size_t sender_, size_t target_, size_t id_) : Message(m, sender_, target_, id_) {}
  // constructor based on the deserialized message
  Ack(Deserializer *d) : Message(d, MsgKind::Ack) {}
};

/**
 * A class that represents a Nack message
 */
class Nack : public Message
{
public:
  // constructor
  Nack(MsgKind m, size_t sender_, size_t target_, size_t id_) : Message(m, sender_, target_, id_) {}
  // constructor that builds a Nack message based on the deserialized message
  Nack(Deserializer *d) : Message(d, MsgKind::Nack) {}
};

/**
 * A class that represents an Put message
 */
class Put : public Message
{
public:
  // constructor
  Put(MsgKind m, size_t sender_, size_t target_, size_t id_) : Message(m, sender_, target_, id_) {}
  // constructor that builds a Put message based on the deserialized message
  Put(Deserializer *d) : Message(d, MsgKind::Put) {}
};

/**
 * A class that represents an Reply message
 */
class Reply : public Message
{
public:
  // constructor
  Reply(MsgKind m, size_t sender_, size_t target_, size_t id_) : Message(m, sender_, target_, id_) {}
  // constructor that builds a Reply message based on the deserialized message
  Reply(Deserializer *d) : Message(d, MsgKind::Reply) {}
};

/**
 * A class that represents a Get message
 */
class Get : public Message
{
public:
  // constructor
  Get(MsgKind m, size_t sender_, size_t target_, size_t id_) : Message(m, sender_, target_, id_) {}
  // constructor that builds a Get message based on the deserialized message
  Get(Deserializer *d) : Message(d, MsgKind::Get) {}
};

/**
 * A class that represents a WaitAndGet message
 */
class WaitAndGet : public Message
{
public:
  // constructor
  WaitAndGet(MsgKind m, size_t sender_, size_t target_, size_t id_) : Message(m, sender_, target_, id_) {}
  // constructor that builds a WaitAndGet message based on the deserialized message
  WaitAndGet(Deserializer *d) : Message(d, MsgKind::WaitAndGet) {}
};

/**
 * A class that represents a Status message
 */
class Status : public Message
{
public:
  String *msg_; // owned
  // constructor
  Status(MsgKind kind_, size_t sender_, size_t target_, size_t id_, String *m) : Message(kind_, sender_, target_, id_)
  {
    msg_ = m;
  }
  // constructor that builds a Status message based on the deserialized message
  Status(Deserializer *d) : Message(d, MsgKind::Status)
  {
    msg_ = d->readString();
  }

  // composition allows prevention of duplicate code. Uses parent code from Message
  void serialize(Serializer *ser)
  {
    Message::serialize(ser);
    ser->write(msg_);
  }
};

/**
 * A class that represents a Kill message
 */
class Kill : public Message
{
public:
  // constructor
  Kill(MsgKind m, size_t sender_, size_t target_, size_t id_) : Message(m, sender_, target_, id_) {}
  // constructor that builds a Kill message based on the deserialized message
  Kill(Deserializer *d) : Message(d, MsgKind::Kill) {}
};

/**
 * A class that represents a Register message
 */
class Register : public Message
{
public:
  // client sock address
  sockaddr_in client_;
  // port number
  size_t port_;

  // constructor
  Register(MsgKind kind_, size_t sender_, size_t target_, size_t id_, sockaddr_in client, size_t port) : Message(kind_, sender_, target_, id_)
  {
    client_ = client;
    port_ = port;
  }
  // constructor that builds a Register message based on the deserialized message
  Register(Deserializer *d) : Message(d, MsgKind::Register)
  {
    client_ = d->readSockAddrIn();
    port_ = d->readSizeT();
  }
  // serialize Register message
  void serialize(Serializer *ser)
  {
    Message::serialize(ser);
    ser->write(client_);
    ser->write(port_);
  }
};

/**
 * A class that represents a Directory message
 */
class Directory : public Message
{
public:
  size_t client_;

  IntArray *ports_; // owned

  // String ** addresses;  // owned; strings owned
  StringArray *addresses_;

  // constructor
  Directory(MsgKind kind_, size_t sender_, size_t target_, size_t id_, size_t client, IntArray *ports, StringArray *addresses) : Message(kind_, sender_, target_, id_)
  {
    client_ = client;
    ports_ = ports;
    addresses_ = addresses;
  }
  // constructor that builds a Directory message based on the deserialized message
  Directory(Deserializer *d) : Message(d, MsgKind::Directory)
  {
    client_ = d->readSizeT();
    ports_ = IntArray::deserializeIntArray(d);
    addresses_ = StringArray::deserializeStringArray(d);
  }
  // serialize a Directory message
  void serialize(Serializer *ser)
  {
    Message::serialize(ser);
    ser->write(client_);
    ports_->serialize(ser);
    addresses_->serialize(ser);
  }
};

/**
 * A class that represents a Data message
 */
class Data : public Message
{
public:
  Value *v_;

  // constructor
  Data(MsgKind kind_, size_t sender_, size_t target_, size_t id_, Value *val_) : Message(kind_, sender_, target_, id_)
  {
    v_ = val_;
  }
  // constructor that builds a Data message based on the deserialized message
  Data(Deserializer *d) : Message(d, MsgKind::Data)
  {
    v_ = Value::deserialize(d);
  }
  // serialize a Data message
  void serialize(Serializer *ser)
  {
    Message::serialize(ser);
    v_->serialize(ser);
  }
};

// deserialize message and return message by type
Message *Message::deserializeMsg(Deserializer *dser)
{
  Message *result = nullptr;
  MsgKind msgkind = (MsgKind)dser->readInt();
  switch (msgkind)
  {
  case MsgKind::Ack:
    result = new Ack(dser);
    break;
  case MsgKind::Nack:
    result = new Nack(dser);
    break;
  case MsgKind::Put:
    result = new Put(dser);
    break;
  case MsgKind::Reply:
    result = new Reply(dser);
    break;
  case MsgKind::Get:
    result = new Get(dser);
    break;
  case MsgKind::WaitAndGet:
    result = new WaitAndGet(dser);
    break;
  case MsgKind::Status:
    result = new Status(dser);
    break;
  case MsgKind::Kill:
    result = new Kill(dser);
    break;
  case MsgKind::Register:
    result = new Register(dser);
    break;
  case MsgKind::Directory:
    result = new Directory(dser);
    break;
  case MsgKind::Data:
    result = new Data(dser);
    break;
  }
  return result;
}
