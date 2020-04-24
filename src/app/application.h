#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../object.h"
#include "../string.h"
#include "../kvstore/kvstore.h"
//#include "../dataframe/dataframe.h"

class Application : public Object
{
public:
  KVStore *kv;
  size_t node_idx_;

  Application(size_t idx, char *ip, size_t num_nodes)
  {
    kv = new KVStore(idx, ip, num_nodes);
    node_idx_ = idx;
  }

  virtual void run_(){};

  size_t this_node()
  {
    return node_idx_;
  }
};