#include "application.h"

class Trivial : public Application
{
public:
  Trivial(size_t idx, char* ip, size_t num_nodes) : Application(idx, ip, num_nodes) {}
  void run_()
  {
    size_t SZ = 100 * 1000;
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
};
