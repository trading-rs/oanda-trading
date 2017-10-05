CC=c++
FLAG=-Wall -std=c++14 -I./include/range-v3/include -I./include/json/src -I./include/fmt -I/usr/local/opt/openssl/include
MAIN=main
TEST=test-exec
OBJS=api.o endpoints.o main.o

.PHONY: test clean all

all: $(OBJS) main

api.o:
	$(CC) $(FLAG) -c ./src/api.hpp

endpoints.o:
	$(CC) $(FLAG) -c ./src/endpoints.hpp

main.o:
	$(CC) $(FLAG) -c ./main.cc

main: main.o
	$(CC) $(FLAG) -L/usr/local/opt/openssl/lib -lssl -lcrypto -lz -lPocoNet -lPocoFoundation -lPocoNetSSL main.o -o $(MAIN)

clean:
	rm *.o $(MAIN)
