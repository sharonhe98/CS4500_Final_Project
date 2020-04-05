#include <cstdlib>
#include <netinet/in.h>
#include "object.h"
#include "string.h"
#include "array.h"

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
  Directory
};

class Message : public Object
{
public:
  MsgKind kind_; // the message kind

  size_t sender_; // the index of the sender node

  size_t target_; // the index of the receiver node

  size_t id_; // an id t unique within the node

  Message(MsgKind m, size_t s, size_t t, size_t i)
  {
    kind_ = m;
    sender_ = s;
    target_ = t;
    id_ = i;
  }

  Message(Deserializer *d, MsgKind k)
  {
    kind_ = k;
    sender_ = d->readSizeT();
    target_ = d->readSizeT();
    id_ = d->readSizeT();
  }

  virtual void serialize(Serializer *ser)
  {
    ser->write((int)kind_);
    ser->write(sender_);
    ser->write(target_);
    ser->write(id_);
  }

  // function declaration
  static Message *deserializeMsg(Deserializer *dser);

};

class Ack : public Message
{
public:
  Ack(MsgKind m, size_t sender_, size_t target_, size_t id_) : Message(m, sender_, target_, id_) {}

  Ack(Deserializer *d) : Message(d, MsgKind::Ack) {}
};

class Nack : public Message
{
public:
  Nack(MsgKind m, size_t sender_, size_t target_, size_t id_) : Message(m, sender_, target_, id_) {}

  Nack(Deserializer *d) : Message(d, MsgKind::Nack) {}
};

class Put : public Message
{
public:
  Put(MsgKind m, size_t sender_, size_t target_, size_t id_) : Message(m, sender_, target_, id_) {}

  Put(Deserializer *d) : Message(d, MsgKind::Put) {}
};

class Reply : public Message
{
public:
  Reply(MsgKind m, size_t sender_, size_t target_, size_t id_) : Message(m, sender_, target_, id_) {}

  Reply(Deserializer *d) : Message(d, MsgKind::Reply) {}
};

class Get : public Message
{
public:
  Get(MsgKind m, size_t sender_, size_t target_, size_t id_) : Message(m, sender_, target_, id_) {}

  Get(Deserializer *d) : Message(d, MsgKind::Get) {}
};

class WaitAndGet : public Message
{
public:
  WaitAndGet(MsgKind m, size_t sender_, size_t target_, size_t id_) : Message(m, sender_, target_, id_) {}

  WaitAndGet(Deserializer *d) : Message(d, MsgKind::WaitAndGet) {}
};

class Status : public Message
{
public:
  String *msg_; // owned

  Status(MsgKind kind_, size_t sender_, size_t target_, size_t id_, String *m) : Message(kind_, sender_, target_, id_)
  {
    msg_ = m;
  }

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

class Kill : public Message
{
public:
  Kill(MsgKind m, size_t sender_, size_t target_, size_t id_) : Message(m, sender_, target_, id_) {}

  Kill(Deserializer *d) : Message(d, MsgKind::Kill) {}
};


class Register : public Message
{
public:
  sockaddr_in client_;

  size_t port_;

  Register(MsgKind kind_, size_t sender_, size_t target_, size_t id_, sockaddr_in client, size_t port) : Message(kind_, sender_, target_, id_)
  {
    client_ = client;
    port_ = port;
  }

  Register(Deserializer *d) : Message(d, MsgKind::Register)
  {
    client_ = d->readSockAddrIn();
    port_ = d->readSizeT();
  }

  void serialize(Serializer *ser)
  {
    Message::serialize(ser);
    ser->write(client_);
    ser->write(port_);
  }
};

class Directory : public Message
{
public:
  size_t client_;

  size_t *ports_; // owned

  // String ** addresses;  // owned; strings owned
  StringArray *addresses_;

  Directory(MsgKind kind_, size_t sender_, size_t target_, size_t id_, size_t client, size_t *ports, StringArray *addresses) : Message(kind_, sender_, target_, id_)
  {
    client_ = client;
    ports_ = ports;
    addresses_ = addresses;
  }

  Directory(Deserializer *d) : Message(d, MsgKind::Directory)
  {
    client_ = d->readSizeT();
    *ports_ = d->readSizeT();
    addresses_ = addresses_->deserializeStringArray(d);
  }

  void serialize(Serializer *ser)
  {
    Message::serialize(ser);
    ser->write(client_);
    ser->write(*ports_);
    addresses_->serialize(ser);
  }
};

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
  }
  return result;
}
