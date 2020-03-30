#include <stdlib.h>
#include <stdio.h>
#include "../src/column.h"

void test_double_column() {
	FloatColumn* fc = new FloatColumn();
	for (size_t i = 0; i < 1000 * 100; i++) {
		fc->push_back(3.0);
	}
	assert(fc->currentChunk_ == 100);
	assert(fc->get(2001) == 3.0);
	fc->set(2001, 3.5);
	assert(fc->get(2001) == 3.5);
	printf("test_double_column succeeded!\n");
}
