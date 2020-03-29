#include <stdlib.h>
#include <stdio.h>
#include <thread>
#include <cassert>
using namespace std;

int add_5(int base, int& sum) {
	int val = base + 5;
	printf("computing %i + 5 = %i\n", base, val);
	sum += val;
	printf("sum is now %i\n", sum);
	return val;
}

int increment(int& sum) {
	sum += 1;
	return sum;
}


int test_small() {

	int sum = 0;

	thread t1(add_5, 1, ref(sum));
	thread t2(add_5, 3, ref(sum));
	thread t3(add_5, 5, ref(sum));

	t1.join();
	t2.join();
	t3.join();

	printf("sum at end is: %i\n", sum);

	assert(sum == 24);
	return 0;
}

void test_large() {
	int sum = 0;

	thread* threads = new thread[10];

	printf("test large initial sum: 0\n");
	for (int i = 0; i < 10; i++) {
		threads[i] = thread(increment, ref(sum));
	}
	for (int i = 0; i < 10; i++) {
		threads[i].join();
	}
	assert(sum == 10);
	printf("sum at the end is: %i\n", sum);
}

int main() {
	test_small();
	test_large();
	return 0;
}
