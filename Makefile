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

start_server:
	g++ -g -Wall -pedantic -std=c++11 -o server test/server.cpp
	./server 127.0.0.1 8000 0

start_client1:
	g++ -g -Wall -pedantic -std=c++11 -o client1 test/client.cpp
	./client1 127.0.0.1 8000 127.0.0.3 8001 1

start_client2:
	g++ -g -Wall -pedantic -std=c++11 -o client2 test/client.cpp
	./client2 127.0.0.1 8000 127.0.0.2 8002 2

trivial_test:
	g++ -g -Wall -pedantic -std=c++11 -o trivial test/trivial_test.cpp	
	./trivial

demo_server:
	g++ -g -Wall -pedantic -std=c++11 -pthread -o demo1 test/demo_server_test.cpp
	./demo1 127.0.0.1 8000 0

demo_client1:
	g++ -g -Wall -pedantic -std=c++11 -pthread -o demo2 test/demo_client_test.cpp
	./demo2 127.0.0.1 8000 127.0.0.2 8001 1

demo_client2:
	g++ -g -Wall -pedantic -std=c++11 -pthread -o demo3 test/demo_client_test.cpp
	./demo2 127.0.0.1 8000 127.0.0.3 8002 2
	
	
