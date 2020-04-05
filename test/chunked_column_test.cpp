#include <stdlib.h>
#include <stdio.h>
#include "../src/column.h"
#define LOG(...) fprintf(stderr, "(" __FILE__ ") " __VA_ARGS__);

void test_double_column() {
	FloatColumn* fc = new FloatColumn();
	for (size_t i = 0; i < 1000 * 100; i++) {
		fc->push_back(3.0);
	}
	assert(fc->currentChunk_ == 99);
	assert(fc->get(2001) == 3.0);
	fc->set(2001, 3.5);
	assert(fc->get(2001) == 3.5);
	printf("test_double_column succeeded!\n");
}

void test_bool_column() {
	BoolColumn* bc = new BoolColumn();
	for (size_t i = 0; i < 1000 * 100; i++) {
		bc->push_back(1);
	}
	assert(bc->currentChunk_ == 99);
	assert(bc->get(2001) == 1);
	bc->set(2001, 0);
	assert(bc->get(2001) == 0);
	printf("test_bool_column succeeded!\n");
}

void test_int_column() {
	IntColumn* ic = new IntColumn();
	for (size_t i = 0; i < 1000 * 100; i++) {
		ic->push_back(23);
	}
	assert(ic->currentChunk_ == 99);
	assert(ic->get(2001) == 23);
	assert(ic->get(0) == 23);
	ic->set(2001, 1);
	assert(ic->get(2001) == 1);
	printf("test_int_column succeeded!\n");
}

void test_str_column() {
	StringColumn* sc = new StringColumn();
	String* hello = new String("hello");
	String* world = new String("world");
	for (size_t i = 0; i < 1000 * 100; i++) {
		sc->push_back(hello);
	}
	assert(sc->currentChunk_ == 99);
	assert(sc->get(2001)->c_str() == hello->c_str());
	assert(sc->get(0)->c_str() == hello->c_str());
	sc->set(2001, world);
	assert(sc->get(2001)->c_str() == world->c_str());
	printf("test_str_column succeeded!\n");
}

int main(int argc, char **argv) {
	test_double_column();
	test_bool_column();
	test_int_column();
	test_str_column();
	LOG("Done.\n");
	return 0;
}
