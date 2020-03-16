  
#include <assert.h>
#include "../src/dataframe.h"


void testArray() {
  StringArray* sa = new StringArray();
  assert(sa->length() == 0);
  assert(sa->size_ == 0);
  printf("test passsed\n");
  String* something = new String("something");
  sa->append(something);
  assert(sa->length() == 1);
  printf("append init passed\n");
  sa->append(something);
  assert(sa->length() == 2);
  printf("append next passed\n");
  sa->append(something);
  assert(sa->length() == 3);
  printf("append final passed\n");
}

void test() {
  Schema s("II");
 // printf("schema cols %i rows %i\n", s.width(), s.length());
  printf("SCHEMA IS BUILT!!\n");
  DataFrame df(s);
  printf("DATAFRAME IS BUILT!\n");
  Row  r(df.get_schema());
  printf("ROW IS BUILLTTTTT!\n");
  for(size_t i = 0; i <  1000 * 1000; i++) {
  //  printf("working?!!\n");
    r.set(0,(int)i);
    //printf("INT SET 1!!\n");
    r.set(1,(int)i+1);
    //printf("INT SET 2!!\n");
    df.add_row(r);
    printf("ADD ROW PASSED!\n");
    printf("row num %i\n", df.nrows());
    printf("col num %i\n", df.ncols());
  }
  
  exit(0);
}


void test2() {
  Schema s("IIFB");
 // printf("schema cols %i rows %i\n", s.width(), s.length());
 // printf("SCHEMA IS BUILT!!\n");
  DataFrame df(s);
  printf("DATAFRAME IS BUILT!\n");
  Row  r(df.get_schema());
  printf("ROW IS BUILLTTTTT!\n");
  String* str = new String("hello world!");
  for(size_t i = 0; i <  2 * 2; i++) {
 //   printf("working?!!\n");
    r.set(0,(int)22);
   // printf("INT SET 1!!\n");
    r.set(1,(int)23);
    //r.set(2, (String*)str);
    r.set(2, (float)2.0);
    r.set(3, (bool)1);
    //printf("INT SET 2!!\n");
    df.add_row(r);

    df.fill_row(i, r);
    printf("ADD ROW PASSED!\n");
    printf("row num %i\n", df.nrows());
    printf("col num %i\n", df.ncols());
    

    //

  }

  df.print();

  
  exit(0);
}


int main(int argc, char **argv) {
  // testArray();
	//test();
	test2();
}
