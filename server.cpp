#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <error.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdio.h>
#include <thread>
#include <time.h>
#include <mutex>
#include <set>

#include "functions.cpp"
#include "constants.cpp"
#include "Connection.cpp"

using namespace std;

mutex mu;
mutex muFile;

// server socket
int servFd;

map<int, bool> proccessing;
map<string, int> readers;
set<string> writers;

// handles SIGINT
void ctrl_c(int);

// sends data to clientFds excluding fd
void sendToAllBut(int fd, char * buffer, int count);

// converts cstring to port
uint16_t readPort(char * txt);

// sets SO_REUSEADDR
void setReuseAddr(int sock);


int main(int argc, char ** argv){
	if(argc != 2) error(1, 0, "Need 1 arg (port)");

	// get and validate port number	
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
	sockaddr_in serverAddr;
	serverAddr.sin_family=AF_INET;
	serverAddr.sin_port=htons((short)port);
	serverAddr.sin_addr={INADDR_ANY};
	
	int res = bind(servFd, (sockaddr*) &serverAddr, sizeof(serverAddr));
	if(res) error(1, errno, "bind failed");
	
	// enter listening mode
	res = listen(servFd, 1);
	if(res) error(1, errno, "listen failed");
	
	int fd =  epoll_create1(0);
	epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = servFd;	 
	epoll_ctl(fd, EPOLL_CTL_ADD, servFd, &event);
	
	while(true){
		//int resultCount = epoll_wait(fd, &event, 1, -1);
		//if(resultCount<0) printf("epoll_wait went wrong\n");
		epoll_wait(fd, &event, 1, -1);
		if( event.events & EPOLLIN){
			int clientFd = event.data.fd;
			if(clientFd == servFd ){
				sockaddr_in clientAddr{0};
				socklen_t clientAddrSize = sizeof(clientAddr);
				int clientFd = accept(servFd, (sockaddr*) &clientAddr, &clientAddrSize);
				if(clientFd == -1) error(1, errno, "accept failed");
				fcntl(clientFd, F_SETFL, O_NONBLOCK, 1);
				event.data.fd = clientFd;	 					
				printf("new connection from: %s:%hu (fd: %d)\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), clientFd);	
				
				mu.lock();
				proccessing[clientFd] = false;
				epoll_ctl(fd, EPOLL_CTL_ADD, clientFd, &event);	
				mu.unlock();
			}
			else{
				if(proccessing.find(clientFd)!=proccessing.end() && proccessing[clientFd]==false){
					mu.lock();
					proccessing[clientFd] = true;	
					mu.unlock();
					thread t([&,clientFd]{
						Connection con(clientFd);
						if(con.readHeaders() < 1) {
							mu.lock();
							proccessing.erase(con.clientFd);
							epoll_ctl(fd, EPOLL_CTL_DEL, con.clientFd, NULL);
							mu.unlock();
							
							printf("removing %d\n", con.clientFd);
							close(con.clientFd);												
						} else {
							con.checkConditions();
							if(con.method=="PUT"){
								con.myPut();
							}									
							con.writeHeaders();
							
							mu.lock();
							proccessing[clientFd] = false;
							mu.unlock();
						}
						//close(clientFd);
						
					});
					t.detach();
					//t.join();
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
	map<int, bool>::iterator it;
	for ( it = proccessing.begin(); it != proccessing.end(); it++ ){
		close(it->first);
	}
	close(servFd);
	printf("\nClosing server\n");
	exit(0);
}