CFLAGS+=-Wall -Wextra -Wpedantic -Wshadow -Wformat-security

server: server.o base64.o string.o
test: test.o base64.o string.o
	cppcheck --inconclusive server.c base64.c string.c test.c

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