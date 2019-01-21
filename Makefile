all:
	g++ xPoll.cpp -o xPoll -pthread -Wall
	./xPoll 1234
clean:
	rm xPoll
