CC=c++
FLAG=-Wall -std=c++14 -I/usr/local/opt/openssl/include -I./include/json/src -I./include/fmt
MAIN=main
TEST=test-exec

.PHONY: test clean all

all: main.o main

main.o:
	$(CC) $(FLAG) -c ./main.cc

main: main.o
	$(CC) $(FLAG) -L/usr/local/opt/openssl/lib -lssl -lcrypto -lz -lPocoNet -lPocoFoundation -lPocoNetSSL main.o -o $(MAIN)

clean:
	rm *.o $(MAIN)
