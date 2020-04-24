#include "application.h"

class Demo : public Application
{
public:
  Key *main = new Key("main", 0);
  Key *verify = new Key("verif", 0);
  Key *check = new Key("ck", 0);

  Demo(size_t idx, char *ip, size_t num_nodes) : Application(idx, ip, num_nodes) {}

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
    std::cout << "in producer\n";
    size_t SZ = 100 * 1000;
    std::cout << "in producer!\n";
    double *vals = new double[SZ];
    double sum = 0;
    for (size_t i = 0; i < SZ; ++i)
      sum += vals[i] = i;

    fromArray(*main, *kv, SZ, vals);
    std::cout << "from array was called!\n";
    fromScalar(*check, *kv, sum);
    std::cout << "from scalar was called!\n";
  }

  void counter()
  {
    std::cout << "in counter!\n";
    DataFrame *v = kv->waitAndGet(main);
    std::cout << "what is in kv?\n";
    size_t sum = 0;
    for (size_t i = 0; i < 100 * 1000; ++i)
      sum += v->get_double(0, i);
    p("The sum is  ").pln(sum);
    fromScalar(*verify, *kv, sum);
  }

  void summarizer()
  {
    std::cout << "in summarizer!\n";
    DataFrame *result = kv->waitAndGet(verify);
    std::cout << "hi\n";
    DataFrame *expected = kv->waitAndGet(check);
    pln(expected->get_double(0, 0) == result->get_double(0, 0) ? "SUCCESS" : "FAILURE");
  }
};
