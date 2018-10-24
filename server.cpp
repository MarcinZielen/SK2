#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <regex>
#include <time.h>

using namespace std;

int main(int argc, char **argv) {
    string host("example.com"),user_agent,method("UNKNOWN");
    string fileName("index.html");
	
    struct sockaddr_in x, xClient;
    socklen_t mySize = sizeof(x);
    x.sin_family = AF_INET;
    x.sin_addr.s_addr = INADDR_ANY;
    x.sin_port = htons(1234);
	int sock = socket(AF_INET, SOCK_STREAM, 0);
    const int one = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    int binding = bind(sock,(struct sockaddr*)&x, mySize);
    int listening = listen(sock, 1);
    int client = accept(sock,(struct sockaddr*)&xClient,&mySize);    
    printf("connection from %s : %d\n", inet_ntoa((struct in_addr)xClient.sin_addr), ntohs(xClient.sin_port));      
	int resNum = 200;
    string resCode;
    char *buffer = new char;
    string line("");
    int it = 0;
	fcntl(client, F_SETFL, O_NONBLOCK);
	sleep(1);
    while(read(client,buffer,1)>0){
		write(1,buffer,1);
        line += buffer;
		try{
        if((int)buffer[0]==10){
            int deli = line.find(": ");
            if(deli > -1){
                //cout << line.substr(0, line.find(": ")) << ":" << line.substr(line.find(":")+2,line.length()-line.find(":")-3) << endl;
                string key = line.substr(0, line.find(": "));
                string val = line.substr(line.find(":")+2,line.length()-line.find(":")-3);
                //if(key=="Host") { fileName = val+fileName; }
            }
            else{
                method = line.substr(0, line.find(" "));
                fileName = line.substr(line.find("/"),line.find("HTTP/1.")-line.find("/")-1); 
				cout << method << endl << fileName << endl;
            }
            line = "";
        }
		}catch(...){}
    }
	switch(resNum){
        case 200:
            resCode = "OK";
    }
	
	char bufor[100];
	int n;
	n = sprintf(bufor, "%s\n", "-----------");
    write(client, bufor, n);
    n = sprintf(bufor, "%s %d %s\n", "HTTP/1.1",resNum,resCode.c_str());
    write(client, bufor, n);
    time_t now;
    time(&now);
    struct tm *t = gmtime(&now);
    char godzina[ 80 ];
    strftime( godzina, 80, "%a, %d %b %Y %X GMT", t );
	n = sprintf(bufor, "Date: %s\n", godzina);
    write(client, bufor, n);
    n = sprintf(bufor, "Server: CustomServer2000\n");
    write(client, bufor, n);
	t->tm_year -= 1;
    strftime( godzina, 80, "%a, %d %b %Y %X GMT", t );
    n = sprintf(bufor, "Expires: %s\n",godzina);
    write(client, bufor, n);
    n = sprintf(bufor, "Cache-Control: no-store, no-cache, must-revalidate\n");
    write(client, bufor, n);
    n = sprintf(bufor, "Keep-Alive: timeout=15, max=100\n");
    write(client, bufor, n);
    n = sprintf(bufor, "Connection: Keep-Alive\n");
    write(client, bufor, n);
	n = sprintf(bufor, "Content-Type: text/html; charset=utf-8\n");
    write(client, bufor, n);
	n = sprintf(bufor, "Connection: Keep-Alive\n");
    write(client, bufor, n);
	int res=open(("."+fileName).c_str(), O_RDONLY);
    if(res<0){
        printf("File error\n");
		printf("%s\n",fileName.c_str());
        return 0;
    }
    while(read(res,buffer,1)>0){
       write(client, buffer, 1);
    }
    close(res);
	try{
		close(client);
	}catch(...){}
    /*
    
    cout << "------------------------------------" << endl;
    cout << "HTTP/1.1 " << resNum << " " << resCode << endl;
    time_t now;
    time(&now);
    struct tm *t = gmtime(&now);
    char godzina[ 80 ];
    strftime( godzina, 80, "%a, %d %b %Y %X GMT", t );
    cout << "Date: " << godzina << endl;
    cout << "Server: CustomServer2000" << endl;
    t->tm_year -= 1;
    strftime( godzina, 80, "%a, %d %b %Y %X GMT", t );
    cout << "Expires: " << godzina << endl;
    cout << "Cache-Control: no-store, no-cache, must-revalidate" << endl;
    cout << "Keep-Alive: timeout=15, max=100" << endl;
    cout << "Connection: Keep-Alive" << endl;
    cout << "Transfer-Encoding: chunked" << endl;
    cout << "Content-Type: text/html; charset=utf-8" << endl;
    printf("%c%c",(char)13,(char)10);
    int res=open(fileName.c_str(), O_RDONLY);
    if(res<0){
        printf("File error\n");
		printf("%s\n",fileName);
        return 0;
    }
    while(read(res,buffer,1)>0){
        printf("%s",buffer);
    }
    close(res);
	try{
		close(client);
	}catch(...){}
	*/
    return 0;
}
