server: server.o base64.o
test: test.o base64.o

.PHONY:
run: server
	./server

.PHONY:
tests: test
	./test

.PHONY:
clean:
	rm -f *.o

.PHONY:
destroy: clean
	rm -f server test