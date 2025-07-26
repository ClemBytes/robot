CFLAGS+=-Wall -Wextra -Wpedantic -Wshadow -Wformat-security -Wwrite-strings

server: server.o base64.o string.o lib_server.o
test: test.o base64.o string.o lib_server.o
	cppcheck --inconclusive server.c base64.c string.c test.c lib_server.c
	gcc $(CFLAGS) -o test test.o base64.o string.o lib_server.o

.PHONY:
run: server
	./server

.PHONY:
tests: test
	./test
	python3 integration_test.py

.PHONY:
clean:
	rm -f *.o

.PHONY:
destroy: clean
	rm -f server test