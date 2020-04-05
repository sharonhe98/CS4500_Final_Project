#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "object.h"
#include "string.h"
#include "kvstore.h"
#include "dataframe.h"

class Application : public Object
{
public:
  KVStore *kv;
  size_t node_idx_;

  Application(size_t idx)
  {
    kv = new KVStore(idx);
    node_idx_ = idx;
  }

  virtual void run_();

  size_t this_node() {
    return node_idx_;
  }
};

class Trivial : public Application
{
public:
  Trivial(size_t idx) : Application(idx) {}
  void run_()
  {
    size_t SZ = 1000 * 1000;
    double *vals = new double[SZ];
    double sum = 0;
    for (size_t i = 0; i < SZ; ++i)
      sum += vals[i] = i;
    Key key("triv", 0);
    DataFrame *df = fromArray(key, *kv, SZ, vals);
    assert(df->get_double(0, 1) == 1);
    DataFrame *df2 = kv->get(&key);
    for (size_t i = 0; i < SZ; ++i)
      sum -= df2->get_double(0, i);
    assert(sum == 0);
    delete df;
    delete df2;
  }

  class Demo : public Application
  {
  public:
    Key *main = new Key("main", 0);
    Key *verify = new Key("verif", 0);
    Key *check = new Key("ck", 0);

    Demo(size_t idx) : Application(idx) {}

    void run_() override
    {
      switch (this_node())
      {
      case 0:
        producer();
        break;
      case 1:
        counter();
        break;
      case 2:
        summarizer();
      }
    }

    void producer()
    {
      size_t SZ = 100 * 1000;
      double *vals = new double[SZ];
      double sum = 0;
      for (size_t i = 0; i < SZ; ++i)
        sum += vals[i] = i;
      fromArray(*main, *kv, SZ, vals);
      fromScalar(*check, *kv, sum);
    }

    void counter() {
    DataFrame* v = kv->waitAndGet(main);
    size_t sum = 0;
    for (size_t i = 0; i < 100*1000; ++i) sum += v->get_double(0,i);
    p("The sum is  ").pln(sum);
    fromScalar(*verify, *kv, sum);
  }

    void summarizer()
    {
      DataFrame *result = kv->waitAndGet(verify);
      DataFrame *expected = kv->waitAndGet(check);
      pln(expected->get_double(0, 0) == result->get_double(0, 0) ? "SUCCESS" : "FAILURE");
    }
  };
};