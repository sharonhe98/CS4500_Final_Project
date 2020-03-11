#include <cstdlib>
#include <netinet/in.h>
#include "object.h"
#include "string.h"

// POSIX/BSD syscalls for file access
#include <unistd.h>

#include <assert.h>


class Serializer {
public:
  char* buf;
  size_t pos;

  Serializer() {
    buf = new char[1024];
    pos = 0;
  }

  ~Serializer(){
    delete [] buf;
  }


void write(size_t size) {
  // copy the size t to buf
  memcpy(buf + pos, &size, sizeof(size_t));
  pos += sizeof(size_t);
      
}

void write(double d) {
  memcpy(buf + pos, &d, sizeof(double));
  pos += sizeof(double);

}

void write(int i) {
  memcpy(buf + pos, &i, sizeof(int));
  pos += sizeof(int);
}

void write(String* s) {
  printf("successfully write string???\n");
  s->c_str();
  printf("the sercar \n");
  size_t strSize = s->allocateSize();
  printf("string size %i\n", s->allocateSize());
  memcpy(buf + pos, s->c_str(), s->allocateSize());
  pos += s->allocateSize();
  printf("successfully write string\n");
}

void write(sockaddr_in s) {
  memcpy(buf + pos, &s, sizeof(sockaddr_in));
  pos += sizeof(sockaddr_in);
}

size_t getPos() {
  printf("pos is: %i\n", pos);
  return pos;
}

char* getSerChar() {
  return buf;
}

};

// serialize a string
class StringArray {
    public:
    String** vals_;
    size_t size_;

   StringArray() {
       size_ = 0;
       vals_ = nullptr;
   }

   // destructor
  ~StringArray() {

  }

  // putting an item at the end of the array
  void append(String* oo)
  {
    // allocate memory for result array with larger size
    String** res = new String*[size_ + 1];
    // if not empty array, put item to end of array
      for (size_t i = 0; i < size_; i++)
      {
        res[i] = vals_[i];
      }
      res[size_] = oo;

    // set elements to the new result array
    vals_ = res;
    // increase size of array
    size_ += 1;
  };

  String* get(size_t nn)
  {
    // if index not within bounds, return nullptr
    if (size_ == 0 || nn >= size_)
    {
      return nullptr;
    }
    // else return element of given index
    else
    {
      for (size_t i = 0; i < size_; i++)
      {
        if (i == nn)
        {
          return vals_[i];
        }
      }
    }
  };

  // set n'th element to the given object
  // returns the replace Object.
  String* set(size_t nn, String* oo)
  {
    // if size not within bounds, return nullptr
    if (size_ == 0 || nn >= size_)
    {
      return nullptr;
    }
    else
    {
      String* res = vals_[nn];
      vals_[nn] = oo;
      return res;
    }
  };


  size_t length() {
      return size_;
  }

  char* serialize(Serializer* &ser) {
    // serialize size of array
    ser->write(size_);
    // for every string, serialize
    for (size_t i = 0; i < length(); i++) {
      String* str = vals_[i];
      ser->write(str);
      ser->getPos();
    }
    return ser->getSerChar();
  }
};

class DoubleArray {
  public:
  // size of array
  size_t size_;
  // elements_ is a pointer to a int* that we stored
  double* vals_;

  // constructor
  DoubleArray()
  {
    // initialize size to 0
    size_ = 0;
    // initialize elements_ to nullptr
    vals_ = nullptr;
  }

  // destructor
  ~DoubleArray() {}

  // putting an item at the end of the array
   // putting an item at the end of the array
  void append(double oo)
  {
    // allocate memory for result array with larger size
    double* res = new double[size_ + 1];
      for (size_t i = 0; i < size_; i++)
      {
        res[i] = vals_[i];
      }
      res[size_] = oo;
    vals_ = res;
    size_ += 1;
  };

  // get the n'th item in the array
  double get(size_t nn)
  {
    // if index not within bounds, return NULL
    if (size_ == 0 || nn >= size_)
    {
      return NULL;
    }
    // else return element of given index
    else
    {
      return vals_[nn];
    }
  }

  // returns the length of the array.
  size_t length()
  {
    return size_;
  }

  char* serialize(Serializer* ser) {
    printf("serialize init pos %i\n", ser->getPos());
    // serialize size
    ser->write(size_);
    printf("serialize size pos %i\n", ser->getPos());
    // for every double in array, serialize
    for (size_t i = 0; i < length(); i++) {
      double dd = vals_[i];
      ser->write(dd);
      ser->getPos();
    }
    return ser->getSerChar();
  }
   

};

class Deserializer {
public:
  char* buf;
  size_t pos;

  Deserializer(char* serialized) {
    buf = serialized;
    pos = 0;
  }

  ~Deserializer(){}

size_t getPos() {
  printf("pos is: %i\n", pos);
  return pos;
}

size_t readSizeT() {
  size_t res = 0;
  memcpy(&res, buf + pos, sizeof(size_t));
  pos += sizeof(size_t);
  return res;
}

String* readString() {
  String* res = new String(buf+pos);
  pos += res->allocateSize();
  printf("deserialize str pos %i\n", pos);
  return res;
 }

double readDouble() {
  double d = 0;
  memcpy(&d, buf + pos, sizeof(double));
  pos += sizeof(double);
  return d;
}

int readInt() {
  int d = 0;
  memcpy(&d, buf + pos, sizeof(int));
  pos += sizeof(int);
  return d;
}

StringArray* deserializeStringArray(){
  // new StringArray 
  StringArray* deStrArray = new StringArray();

  // deserialize size at char buffer position 0
  // amount of bytes of the size field of array (8 bytes)
  size_t sizeArr = readSizeT();
    
  for(size_t i = 0; i < sizeArr;  i++) {
    // get the new position offset
    size_t current_offset = getPos();
    printf("new offset %i get pos %i\n", current_offset, getPos());
    // deserialize string at the offset position
    String* deserStr = readString();
    printf("the serialized str %s\n", &buf[current_offset]);
    // add string to result array
    deStrArray->append(deserStr);
  }
    
  return deStrArray;
}

DoubleArray* deserializeDoubleArray(){
   
    // create new double array
    DoubleArray* resArr = new DoubleArray();

    // deserialize size at char buffer position 0
    size_t sizeArr = readSizeT();
    
    for (size_t i = 0; i < sizeArr; i++) {
      size_t current_offset = getPos();
      // deserialize double
      // printf("the serialized double %s\n", &buf[current_offset]);
      double d = readDouble();
      printf("the deserialized double %lf\n", d);
      resArr->append(d);
  }
  return resArr;    
  } 

sockaddr_in readSockAddrIn() {
  sockaddr_in sockAdd;
  memcpy(&sockAdd, buf + pos, sizeof(sockaddr_in));
  pos += sizeof(sockaddr_in);
  return sockAdd;
}

char* getSerChar() {
  return buf;
}

};

 
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
        case MsgKind::Directory: result = new Directory(msgkind, dser->readSizeT(), dser->readSizeT(), dser->readSizeT(), dser->readSizeT(), (size_t *)dser->readSizeT(), dser->deserializeStringArray());
        break;
      }
      return result;
}
