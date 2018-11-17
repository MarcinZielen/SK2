#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <error.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <poll.h> 
#include <thread>
#include <unordered_set>
#include <signal.h>
#include <fcntl.h>

// server socket
int servFd;

// client sockets
std::unordered_set<int> clientFds;

// handles SIGINT
void ctrl_c(int);

// sends data to clientFds excluding fd
void sendToAllBut(int fd, char * buffer, int count);

// converts cstring to port
uint16_t readPort(char * txt);

// sets SO_REUSEADDR
void setReuseAddr(int sock);

int main(int argc, char ** argv){
	// get and validate port number
	if(argc != 2) error(1, 0, "Need 1 arg (port)");
	auto port = readPort(argv[1]);
	
	// create socket
	servFd = socket(AF_INET, SOCK_STREAM, 0);
	if(servFd == -1) error(1, errno, "socket failed");
	
	// graceful ctrl+c exit
	signal(SIGINT, ctrl_c);
	// prevent dead sockets from throwing pipe errors on write
	signal(SIGPIPE, SIG_IGN);
	
	setReuseAddr(servFd);
	
	// bind to any address and port provided in arguments
	sockaddr_in serverAddr{.sin_family=AF_INET, .sin_port=htons((short)port), .sin_addr={INADDR_ANY}};
	int res = bind(servFd, (sockaddr*) &serverAddr, sizeof(serverAddr));
	if(res) error(1, errno, "bind failed");
	
	// enter listening mode
	res = listen(servFd, 1);
	if(res) error(1, errno, "listen failed");
	// read a message
	char buffer[255];
	
	/*
	pollfd *pollTab;
	pollTab=new pollfd[1];
	pollTab[0].fd=servFd;
	pollTab[0].events=POLLIN;
	
	while(true){
		int mySize = clientFds.size()+1;
		int gotowe = poll(pollTab, mySize, -1);
		for(int i=0;i<mySize;i++) {
			pollfd opis = pollTab[i];
			if(opis.revents & POLLIN) {				
				if(opis.fd == servFd) {
					sockaddr_in clientAddr{0};
					socklen_t clientAddrSize = sizeof(clientAddr);
					int clientFd = accept(servFd, (sockaddr*) &clientAddr, &clientAddrSize);
					if(clientFd == -1) error(1, errno, "accept failed");
					fcntl(clientFd, F_SETFL, O_NONBLOCK, 1);
					clientFds.insert(clientFd);
					pollTab=new pollfd[clientFds.size()+1];
					pollTab[0].fd=servFd;
					pollTab[0].events=POLLIN;
					int it=1;
					for(int j : clientFds){
						pollTab[it].fd=j;
						pollTab[it++].events=POLLIN;
					}						
					printf("new connection from: %s:%hu (fd: %d)\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), clientFd);	
				}
				else{
					int clientFd = opis.fd;
					int count = read(clientFd, buffer, 255);		
					if(count < 1) {
						printf("removing %d\n", clientFd);
						clientFds.erase(clientFd);
						close(clientFd);
						pollTab=new pollfd[clientFds.size()+1];
						pollTab[0].fd=servFd;
						pollTab[0].events=POLLIN;
						int it=1;
						for(int j : clientFds){
							pollTab[it].fd=j;
							pollTab[it++].events=POLLIN;
						}
						continue;
					} else {
						sendToAllBut(clientFd, buffer, count);
					}
				}
			}
		}		
	}
	*/
	
	int fd =  epoll_create1(0);
	epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = servFd;	 
	epoll_ctl(fd, EPOLL_CTL_ADD, servFd, &event);
	
	while(true){
		int resultCount = epoll_wait(fd, &event, 1, -1);	 
		if( event.events & EPOLLIN){
			int opis = event.data.fd;
			if(opis == servFd ){
				sockaddr_in clientAddr{0};
				socklen_t clientAddrSize = sizeof(clientAddr);
				int clientFd = accept(servFd, (sockaddr*) &clientAddr, &clientAddrSize);
				if(clientFd == -1) error(1, errno, "accept failed");
				fcntl(clientFd, F_SETFL, O_NONBLOCK, 1);
				clientFds.insert(clientFd);
				event.data.fd = clientFd;	 
				epoll_ctl(fd, EPOLL_CTL_ADD, clientFd, &event);						
				printf("new connection from: %s:%hu (fd: %d)\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), clientFd);	
			}
			else{
				int clientFd = opis;
				int count = read(clientFd, buffer, 255);		
				if(count < 1) {
					printf("removing %d\n", clientFd);
					clientFds.erase(clientFd);
					close(clientFd);
					epoll_ctl(fd, EPOLL_CTL_DEL, clientFd, NULL);					
					continue;
				} else {
					sendToAllBut(clientFd, buffer, count);
				}
			}
		}
	}
	
	return 0;
}

uint16_t readPort(char * txt){
	char * ptr;
	auto port = strtol(txt, &ptr, 10);
	if(*ptr!=0 || port<1 || (port>((1<<16)-1))) error(1,0,"illegal argument %s", txt);
	return port;
}

void setReuseAddr(int sock){
	const int one = 1;
	int res = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
	if(res) error(1,errno, "setsockopt failed");
}

void ctrl_c(int){
	for(int clientFd : clientFds)
		close(clientFd);
	close(servFd);
	printf("Closing server\n");
	exit(0);
}

void sendToAllBut(int fd, char * buffer, int count){
	int res;
	decltype(clientFds) bad;
	for(int clientFd : clientFds){
		if(clientFd == fd) continue;
		res = write(clientFd, buffer, count);
		if(res!=count)
			bad.insert(clientFd);
	}
	for(int clientFd : bad){
		printf("removing %d\n", clientFd);
		clientFds.erase(clientFd);
		close(clientFd);
	}
}