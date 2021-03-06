#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

using namespace std;

int main(int argc, char **argv) {
	if(argc<2){
		cout << "Usage: " << argv[0] << " host port" << endl;
		return -5;
	}
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
       fprintf(stderr, "%s: Can't create a socket.\n", argv[0]);
       shutdown(sock,SHUT_RDWR);
       close(sock);       
       exit(1);
    }
    sockaddr_in x;
    x.sin_family = AF_INET;
    x.sin_port = htons(atoi(argv[2]));
    //x.sin_addr.s_addr = inet_addr("127.0.0.1");
    inet_aton(argv[1], &x.sin_addr);
    int conn = connect(sock,(struct sockaddr*)&x,sizeof(x));
    if (conn < 0)
    {
       fprintf(stderr, "%s: Can't connect to socket.\n", argv[0]);
       shutdown(sock,SHUT_RDWR);
       close(sock);
       
       exit(1);
    }
	char* bufor = new char[20];
	//int n = sprintf(bufor, "%s.txt", argv[3]);
    //int fileIn=open(bufor, O_RDONLY);
	int fileIn=open("post.txt", O_RDONLY);
    char *buffer = new char;
	while(read(fileIn,buffer,1)>0){
        write(sock,buffer,1);
		write(1,buffer,1);
    }
	//printf("start\n");
	sleep(3);
	//printf("end\n");
	int flag;
	fcntl(sock, F_SETFL, O_NONBLOCK);
	while(read(sock,buffer,1)>0){
//this works on unixlab but doesnt work on pi
		//if((int)buffer[0]==10 || (int)buffer[0]==13) { if(++flag==4) break; }
//this works on pi but doesnt work on unixlab
		if((int)buffer[0]==10 || (int)buffer[0]==13) { if(++flag==2) break; }
		else flag = 0;
        write(1,buffer,1);
    }
	flag=0;
	while(read(sock,buffer,1)>0){
        write(1,buffer,1);
    }
	
    shutdown(sock,SHUT_RDWR);
    close(sock);
    return 0;
}
