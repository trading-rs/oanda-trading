CC=c++
FLAG=-Wall -std=c++14
INCLUDE=-I./include/range-v3/include -I./include/json/src -I./include/fmt -I/usr/local/opt/openssl/include -I./include/catch/single_include
LIB=-L/usr/local/opt/openssl/lib
LINK=-lssl -lcrypto -lz -lPocoNet -lPocoFoundation -lPocoNetSSL
MAIN=main
TEST=test-exec
OBJS=api.o endpoints.o main.o

.PHONY: test clean all

all: $(OBJS) test main

api.o:
	$(CC) $(FLAG) $(INCLUDE) -c ./src/api.hpp

endpoints.o:
	$(CC) $(FLAG) $(INCLUDE) -c ./src/endpoints.hpp

main.o:
	$(CC) $(FLAG) $(INCLUDE) -c ./main.cc

main: main.o
	$(CC) $(FLAG) $(INCLUDE) $(LIB) $(LINK) main.o -o $(MAIN)

test:
	$(CC) $(FLAG) $(INCLUDE) $(LIB) $(LINK) ./test/test.cc -o $(TEST) && ./$(TEST)

clean:
	rm *.o $(MAIN)
