#include <cstdlib>
#include <netinet/in.h>
#include "object.h"
#include "string.h"
#include "array.h"

enum class MsgKind { Ack, Nack, Put,

  Reply,  Get, WaitAndGet, Status,

  Kill,   Register,  Directory };

class Message : public Object {
  public:
    MsgKind kind_;  // the message kind

    size_t sender_; // the index of the sender node

    size_t target_; // the index of the receiver node

    size_t id_;     // an id t unique within the node

    Message(MsgKind m, size_t s, size_t t, size_t i) {
      kind_ = m;
      sender_ = s;
      target_ = t;
      id_ = i;
    }

    virtual void serialize(Serializer* ser) {
      ser->write((int)kind_);
      ser->write(sender_);
      ser->write(target_);
      ser->write(id_);
    }

    // function declaration
    static Message* deserializeMsg(Deserializer* dser);

    // virtual Message* deserializeMessage(Deserializer* dser) {
    //   char* serArr = dser->getSerChar();
    //   int msgkind = dser->readInt();
    //   size_t sender = dser->readSizeT();
    //   size_t target = dser->readSizeT();
    //   size_t id = dser->readSizeT();
    //   return new Message((MsgKind)msgkind, sender, target, id);
    // }

};

class Ack : public Message {
  public:

  Ack(MsgKind m , size_t sender_, size_t target_, size_t id_) : Message(m, sender_, target_, id_) {}
};

class Status : public Message {
  public:

   String* msg_; // owned

  Status(MsgKind kind_, size_t sender_, size_t target_, size_t id_, String* msg_) : Message(kind_, sender_, target_, id_) {}

  // composition allows prevention of duplicate code. Uses parent code from Message
   void serialize(Serializer* ser) {
      Message::serialize(ser);
      printf("pos after msg serial is %i\n", ser->getPos());
      ser->write(msg_);
      printf("pos after msg serial is %i\n", ser->getPos());
    }

    Status* deserialize(Deserializer* dser) {
      Message::deserializeMsg(dser);
    }
};

class Register : public Message {
  public:

    sockaddr_in client;

    size_t port;

    Register(MsgKind kind_, size_t sender_, size_t target_, size_t id_, sockaddr_in client, size_t port) : Message(kind_, sender_, target_, id_) {}

    void serialize(Serializer* ser) {
      Message::serialize(ser);
      ser->write(client);
      ser->write(port);
    }
};

class Directory : public Message {
  public:
   size_t client;

   size_t * ports;  // owned

   // String ** addresses;  // owned; strings owned
   StringArray* addresses;

   Directory(MsgKind kind_, size_t sender_, size_t target_, size_t id_, size_t client, size_t * ports, StringArray* addresses) : Message(kind_, sender_, target_, id_) {}

   void serialize(Serializer* ser) {
     Message::serialize(ser);
     ser->write(client);
     ser->write(*ports);
     // use stringArray??
     addresses->serialize(ser);
    }

};

Message* Message::deserializeMsg(Deserializer* dser) {
  Message* result = nullptr;
      MsgKind msgkind = (MsgKind)dser->readInt();
      printf("msgKind is: %i\n", (int)msgkind);
      switch (msgkind) {
        case MsgKind::Ack: result = new Ack(msgkind, dser->readSizeT(), dser->readSizeT(), dser->readSizeT());
        break;
        case MsgKind::Status: result = new Status(msgkind, dser->readSizeT(), dser->readSizeT(), dser->readSizeT(), dser->readString());
        break;
        case MsgKind::Register: result = new Register(msgkind, dser->readSizeT(), dser->readSizeT(), dser->readSizeT(), dser->readSockAddrIn(), dser->readSizeT());
        break;
        // case MsgKind::Directory: result = new Directory(msgkind, dser->readSizeT(), dser->readSizeT(), dser->readSizeT(), dser->readSizeT(), (size_t *)dser->readSizeT(), dser->deserializeStringArray());
        // break;
      }
      return result;
}
