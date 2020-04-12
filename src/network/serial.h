#include <cstdlib>
#include <netinet/in.h>
#include "../object.h"
#include "../string.h"

// POSIX/BSD syscalls for file access
#include <unistd.h>

#include <assert.h>

class Serializer
{
public:
  char *buf;
  size_t pos;
  size_t capacity;

  Serializer()
  {
    buf = new char[capacity = 1024];
    pos = 0;
  }

  ~Serializer()
  {
    delete[] buf;
  }

  // resize buffer when capacity is reached
  void resize(size_t sizeOf) {

    if (pos + sizeOf >= capacity) {
      printf("sizeof %zu pos: %zu\n", capacity, pos);
      capacity *= 2;
      char *oldV = buf;
      buf = new char[capacity];
      memcpy(buf, oldV, pos);
      delete[] oldV;
    }
    
  }

  void write(size_t size)
  {
    resize(sizeof(size_t));
    // copy the size t to buf
    memcpy(buf + pos, &size, sizeof(size_t));
    pos += sizeof(size_t);
  }

  void write(double d)
  {
    resize(sizeof(double));
    memcpy(buf + pos, &d, sizeof(double));
    pos += sizeof(double);
  }

  void write(int i)
  {
    resize(sizeof(int));
    memcpy(buf + pos, &i, sizeof(int));
    pos += sizeof(int);
  }

  void write(bool b)
  {
    resize(sizeof(bool));
    memcpy(buf + pos, &b, sizeof(bool));
    pos += sizeof(bool);
  }

  void write(char c)
  {
    resize(sizeof(char));
    memcpy(buf + pos, &c, sizeof(char));
    pos += sizeof(char);
  }

  void write(char *c)
  {
    resize(sizeof(char *));
    memcpy(buf + pos, &c, sizeof(char *));
    pos += sizeof(char *);
  }

  void write(String *s)
  {
    resize(s->allocateSize());
    memcpy(buf + pos, s->c_str(), s->allocateSize());
    pos += s->allocateSize();
  }

  void write(sockaddr_in s)
  {
    resize(sizeof(sockaddr_in));
    memcpy(buf + pos, &s, sizeof(sockaddr_in));
    pos += sizeof(sockaddr_in);
  }

  size_t getPos()
  {
    return pos;
  }

  char *getSerChar()
  {
    printf("buf is %x\n", *((unsigned int*)buf));
    return buf;
  }
};

class Deserializer
{
public:
  char *buf;
  size_t pos;

  Deserializer(char *serialized)
  {
    buf = serialized;
    pos = 0;
  }

  ~Deserializer() {}

  size_t getPos()
  {
    return pos;
  }

  size_t readSizeT()
  {
    size_t res = 0;
    memcpy(&res, buf + pos, sizeof(size_t));
    pos += sizeof(size_t);
    return res;
  }

  char readChar()
  {
    char res = 0;
    memcpy(&res, buf + pos, sizeof(char));
    pos += sizeof(char);
    return res;
  }

  char *readChars()
  {
    char *res;
    memcpy(&res, buf + pos, sizeof(char *));
    pos += sizeof(char *);
    return res;
  }

  String *readString()
  {
    String *res = new String(buf + pos);
    pos += res->allocateSize();
    return res;
  }

  double readDouble()
  {
    double d = 0;
    memcpy(&d, buf + pos, sizeof(double));
    pos += sizeof(double);
    return d;
  }

  int readInt()
  {
    int i = 0;
    memcpy(&i, buf + pos, sizeof(int));
    pos += sizeof(int);
    return i;
  }

  bool readBool()
  {
    bool b = 0;
    memcpy(&b, buf + pos, sizeof(bool));
    pos += sizeof(bool);
    return b;
  }

  sockaddr_in readSockAddrIn()
  {
    sockaddr_in sockAdd;
    memcpy(&sockAdd, buf + pos, sizeof(sockaddr_in));
    pos += sizeof(sockaddr_in);
    return sockAdd;
  }

  char *getSerChar()
  {
    return buf;
  }
};