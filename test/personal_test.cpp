
#include <assert.h>
#include "../src/kvstore/kvstore.h"
#include "../src/dataframe/sorer.h"

void testArray()
{
	StringArray *sa = new StringArray();
	assert(sa->length() == 0);
	assert(sa->size_ == 0);
	String *something = new String("something");
	sa->append(something);
	assert(sa->length() == 1);
	sa->append(something);
	assert(sa->length() == 2);
	sa->append(something);
	assert(sa->length() == 3);
	assert(sa->get(0)->c_str() == something->c_str());
	printf("test array passsed\n");
}

void test()
{
	Schema s("II");
	// printf("schema cols %i rows %i\n", s.width(), s.length());
	DataFrame df(s);
	Row r(df.get_schema());
	for (size_t i = 0; i < 100 * 1000; i++)
	{
		r.set(0, (int)i);
		r.set(1, (int)i + 1);
		df.add_row(r);
		printf("ADD ROW PASSED!\n");
	}

	exit(0);
}

void test2()
{
	Schema s("IIFBS");
	DataFrame df(s);
	Row r(df.get_schema());
	String *str = new String("hello world!");
	for (size_t i = 0; i < 2 * 2; i++)
	{
		r.set(0, (int)22);
		r.set(1, (int)23);
		r.set(2, (float)2.0);
		r.set(3, (bool)1);
		r.set(4, (String *)str);
		df.add_row(r);
	}

	df.print();
	exit(0);
}

void test3()
{
	Schema s("IIF");
	DataFrame df(s);
	Row r(df.get_schema());
	for (size_t i = 0; i < 2 * 2; i++)
	{
		r.set(0, (int)22);
		r.set(1, (int)23);
		r.set(2, (float)23.5);
		df.add_row(r);
	}

	df.print();
	//exit(0);
}

void testConcat()
{
	String *hello = new String("hello");
	String *world = new String("world");
	String *helloworld = hello->concat(world);
	String *expected = new String("helloworld");
	assert(helloworld->equals(expected));
	printf("test concat passed!\n");
}

void testGetSchema()
{
	FILE *f = fopen("test/data.sor", "r");
	SOR *sor = new SOR();
	char *schemaFromFile = sor->getSchema(f, 0, 1000000);
	assert(strcmp(schemaFromFile, "BISFI") == 0);
	printf("get schema passed!\n");
}

void test4()
{
	FILE *f = fopen("test/data.sor", "r");
	SOR *sor = new SOR();
	char *schemaFromFile = sor->getSchema(f, 0, 1000000);
	Schema s(schemaFromFile);

	DataFrame *df = sor->setDataFrame(f, 0, 100000);
	df->print();
	printf("Build DF from file passed!\n");
}

void test5()
{
	FILE *f = fopen("../src/3.sor", "r");
	SOR *sor = new SOR();
	DataFrame *df = sor->setDataFrame(f, 0, 10000000);
	df->print();
}

void testFloatArray()
{
	FloatArray *fa = new FloatArray();
	assert(fa->length() == 0);
	for (size_t i = 0; i < 1000; i++)
	{
		fa->append(3.5);
	}
	assert(fa->length() == 1000);
	assert(fa->get(0) == 3.5);
	printf("test float array functions pass!\n");
}

void testFloatColumn()
{
	FloatColumn *fa = new FloatColumn();
	assert(fa->size() == 0);
	for (size_t i = 0; i < 1000; i++)
	{
		fa->push_back(3.5);
	}
	assert(fa->size() == 1000);
	assert(fa->get(0) == 3.5);
	printf("test float col functions pass!\n");
}

void testFloatsForRow()
{
	Schema s("FFF");
	Row *row = new Row(s);
	assert(row->width() == 3);
	row->set(0, (float)3.5);
	assert(row->get_float(0) == 3.5);
	printf("testfloatsfor row passed!\n");
}

void testBoolArray()
{
	BoolArray *fa = new BoolArray();
	assert(fa->length() == 0);
	for (size_t i = 0; i < 1000; i++)
	{
		fa->append(0);
	}
	assert(fa->length() == 1000);
	assert(fa->get(0) == 0);
	printf("test bool array functions pass!\n");
}

void testBoolColumn()
{
	BoolColumn *fa = new BoolColumn();
	assert(fa->size() == 0);
	for (size_t i = 0; i < 1000; i++)
	{
		fa->push_back(1);
	}
	assert(fa->size() == 1000);
	assert(fa->get(0) == 1);
	printf("test bool col functions pass!\n");
}

void testStringArray()
{
	Array *fa = new Array();
	assert(fa->length() == 0);
	String *s = new String("hello world!");
	for (size_t i = 0; i < 1000; i++)
	{
		fa->append(s);
	}
	assert(fa->length() == 1000);
	assert(dynamic_cast<String *>(fa->get_(0))->equals(s));
	printf("test str array functions pass!\n");
}

void testStringColumn()
{
	StringColumn *fa = new StringColumn();
	String *s = new String("hello world!");
	assert(fa->size() == 0);
	for (size_t i = 0; i < 1000; i++)
	{
		fa->push_back(s);
	}
	assert(fa->size() == 1000);
	assert(fa->get(0)->equals(s));
	printf("test str col functions pass!\n");
}

void testStringForRow()
{
	Schema s("SSS");
	Row *row = new Row(s);
	String *str = new String("hello world!");
	assert(row->width() == 3);
	row->set(0, (String *)str);
	assert(row->get_string(0) == str);
	printf("teststringsfor row passed!\n");
}

void testBoolsForRow()
{
	Schema s("BBB");
	Row *row = new Row(s);
	assert(row->width() == 3);
	row->set(0, (bool)1);
	assert(row->get_bool(0) == 1);
	printf("testboolsfor row passed!\n");
}

int main(int argc, char **argv)
{
	testArray();
	// test();
	// test2();
	testConcat();
	test4();
	testFloatArray();
	testFloatsForRow();
	testBoolArray();
	testBoolColumn();
	testBoolsForRow();
	testStringArray();
	testStringColumn();
	testStringForRow();
	testGetSchema();
	// test5();
}
