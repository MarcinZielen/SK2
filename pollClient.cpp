#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <error.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <poll.h> 
#include <thread>
#include <vector>

ssize_t readData(int fd, char * buffer, ssize_t buffsize){
	auto ret = read(fd, buffer, buffsize);
	if(ret==-1) error(1,errno, "read failed on descriptor %d", fd);
	return ret;
}

void writeData(int fd, char * buffer, ssize_t count){
	auto ret = write(fd, buffer, count);
	if(ret==-1) error(1, errno, "write failed on descriptor %d", fd);
	if(ret!=count) error(0, errno, "wrote less than requested to descriptor %d (%ld/%ld)", fd, count, ret);
}

int main(int argc, char ** argv){
	if(argc!=3) error(1,0,"Need 2 args");
	
	// Resolve arguments to IPv4 address with a port number
	addrinfo *resolved, hints={.ai_flags=0, .ai_family=AF_INET, .ai_socktype=SOCK_STREAM};
	int res = getaddrinfo(argv[1], argv[2], &hints, &resolved);
	if(res || !resolved) error(1, 0, "getaddrinfo: %s", gai_strerror(res));
	
	// create socket
	int sock = socket(resolved->ai_family, resolved->ai_socktype, 0);
	if(sock == -1) error(1, errno, "socket failed");
	
	// attept to connect
	res = connect(sock, resolved->ai_addr, resolved->ai_addrlen);
	if(res) error(1, errno, "connect failed");
	
	// free memory
	freeaddrinfo(resolved);
	
	ssize_t bufsize1 = 255, received1;
	char buffer1[bufsize1];
	ssize_t bufsize2 = 255, received2;
	char buffer2[bufsize2];
	
	/*
	int fd =  epoll_create1(0);
	epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = sock;	 
	epoll_ctl(fd, EPOLL_CTL_ADD, sock, &event);
	event.data.fd = 0;	 
	epoll_ctl(fd, EPOLL_CTL_ADD, 0, &event);
	
	while(true){
		int resultCount = epoll_wait(fd, &event, 1, -1);	 
		if( event.events & EPOLLIN){
			if(event.data.fd == sock ){
				received1 = readData(sock, buffer1, bufsize1);
				writeData(1, buffer1, received1);
			}
			if(event.data.fd == 0 ){
				received2 = readData(0, buffer2, bufsize2);
				writeData(sock, buffer2, received2);
			}
		}
	}	
	*/
	
	pollfd *pollTab;
	pollTab=new pollfd[2];
	pollTab[0].fd=sock;
	pollTab[0].events=POLLIN;
	pollTab[1].fd=0;
	pollTab[1].events=POLLIN;
	
	while(true){
		int gotowe = poll(pollTab, 2, -1);
		for(int i=0;i<2;i++) {
			pollfd opis = pollTab[i];
			if(opis.revents & POLLIN) {
				if(opis.fd == sock) {
					received1 = readData(sock, buffer1, bufsize1);
					writeData(1, buffer1, received1);
				}
				if(opis.fd == 0) {
					received2 = readData(0, buffer2, bufsize2);
					writeData(sock, buffer2, received2);
				}
			}
		}
	}
		
	
	close(sock);
	
	return 0;
}
