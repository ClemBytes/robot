CFLAGS+=-Wall -Wextra

server: server.o base64.o string.o
test: test.o base64.o string.o

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