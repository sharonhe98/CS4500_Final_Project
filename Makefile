build:
	g++ -g -Wall -std=c++11 test/personal_test.cpp -o a.out

run:
	./a.out

valgrind:
	valgrind ./a.out

serial:
	g++ -g -Wall -pedantic -std=c++11 test/serial_test.cpp -o serial

chunked_test:
	g++ -g -Wall -pedantic -std=c++11 test/chunked_column_test.cpp -o chunked

kv:
	g++ -g -Wall -pedantic -std=c++11 -o kv_test test/kvstore_test.cpp

df_from:
	g++ -g -Wall -pedantic -std=c++11 -o from test/dataframe_from_test.cpp	
