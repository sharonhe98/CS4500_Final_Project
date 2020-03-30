#include <cstdlib>
#include <netinet/in.h>
#include "object.h"
#include "string.h"

// POSIX/BSD syscalls for file access
#include <unistd.h>

#include <assert.h>

class Serializer
{
public:
  char *buf;
  size_t pos;

  Serializer()
  {
    buf = new char[1024];
    pos = 0;
  }

  ~Serializer()
  {
    delete[] buf;
  }

  void write(size_t size)
  {
    // copy the size t to buf
    memcpy(buf + pos, &size, sizeof(size_t));
    pos += sizeof(size_t);
  }

  void write(double d)
  {
    memcpy(buf + pos, &d, sizeof(double));
    pos += sizeof(double);
  }

  void write(int i)
  {
    memcpy(buf + pos, &i, sizeof(int));
    pos += sizeof(int);
  }

  void write(bool b)
  {
    memcpy(buf + pos, &b, sizeof(bool));
    pos += sizeof(bool);
  }

  void write(char c)
  {
    memcpy(buf + pos, &c, sizeof(char));
    pos += sizeof(char);
  }

  void write(char *c)
  {
    memcpy(buf + pos, &c, sizeof(char *));
    pos += sizeof(char *);
  }

  void write(String *s)
  {
    s->c_str();
    memcpy(buf + pos, s->c_str(), s->allocateSize());
    pos += s->allocateSize();
  }

  void write(sockaddr_in s)
  {
    memcpy(buf + pos, &s, sizeof(sockaddr_in));
    pos += sizeof(sockaddr_in);
  }

  size_t getPos()
  {
    return pos;
  }

  char *getSerChar()
  {
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

  int readBool()
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