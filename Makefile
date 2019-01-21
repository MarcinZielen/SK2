all:
	g++ server.cpp -o server -pthread -Wall
	./server 1234
clean:
	rm server
