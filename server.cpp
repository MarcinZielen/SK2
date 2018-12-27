#include "variables.cpp"
#include "connection.cpp"

using namespace std;

int main(int argc, char **argv) {
	if(argc<2){
		cout << "Usage: " << argv[0] << " [port]" << endl;
		return -5;
	}
	loadAll();
	int client = startListening(argv[1]);//port
	if(client<0) return client;
	
	//sleep(1);
	//readHeaders(client);
	if(method=="PUT" || method=="POST"){
		body = readBody(client);
	}
//	resNum = checkConditions();
	resNum = 200;
	resCode=resCodes[resNum];
	
	if(method=="DELETE"){
		string tmp = (host+"/"+fileName);
		int res=open(tmp.c_str(), O_RDONLY);
		close(res);
		if(res<0){
			printf("Delete error\n");
			printf("%s\n",tmp.c_str());
		}
		else remove(tmp.c_str());
		n = sprintf(bufor, "HTTP/1.1 200 OK\n");
		write(client, bufor, n);
//		n = sprintf(bufor, "Date: %s\n", godzina);
		write(client, bufor, n);
		res=open("delete.html", O_RDONLY);
		if(res<0){
			printf("File error\n");
			printf("%s\n",fileName.c_str());
			return 0;
		}
		while(read(res,buffer,1)>0){
		   write(client, buffer, 1);
		}
		close(res);
		
	}

	writeHeaders(client);

	//if(method=="GET" || method=="PUT" || method=="POST"){		
	if(method=="NOPE"){		
//HARDCODED TO WORK ALWAYS
		n = sprintf(bufor, "HTTP/1.1 200 OK\n");
		write(client, bufor, n);
		write(1, bufor, n);
//NOT HARDCODED TO WORK	ALWAYS
		//n = sprintf(bufor, "HTTP/1.1 %d %s\n",resNum,resCode.c_str());
		//write(client, bufor, n);
		//write(1, bufor, n);
		
		//n = sprintf(bufor, "Date: %s\n", godzina);
		write(client, bufor, n);
		write(1, bufor, n);
		
		n = sprintf(bufor, "Server: CustomServer2000\n");
		write(client, bufor, n);
		write(1, bufor, n);
		
		n = sprintf(bufor, "Content-Language: en\n");
		write(client, bufor, n);
		write(1, bufor, n);
		
		n = sprintf(bufor, "Access-Control-Allow-Origin: *\n");
		write(client, bufor, n);
		write(1, bufor, n);
		
		//TODO
		//n = sprintf(bufor, "Content-Type: application/x-www-form-urlencoded; charset=utf-8\n");
		//write(client, bufor, n);	
		//write(1, bufor, n);

//POST		
		//n = sprintf(bufor, "\n{\"login\":\"baton96\"}\n");
		//write(client, bufor, n);
		//write(1, bufor, n);
		
		//TODO
		//wildcard not implemented everywhere
		//n = sprintf(bufor, "Access-Control-Allow-Headers: *\n");
		//write(client, bufor, n);
		
		//n = sprintf(bufor, "Access-Control-Allow-Credentials: true\n");
		//write(client, bufor, n);
		//or
		//n = sprintf(bufor, "Access-Control-Allow-Origin: *\n");
		//write(client, bufor, n);
		
		//n = sprintf(bufor, "Access-Control-Allow-Methods: POST, GET, OPTIONS, DELETE, PUT\n");
		//write(client, bufor, n);
		
		//n = sprintf(bufor, "Access-Control-Max-Age: 6000\n");
		//write(client, bufor, n);
		
		//CORS
		//if(resNum==405){
		//	n = sprintf(bufor, "Allow: POST, GET, OPTIONS, DELETE, PUT\n");
		//	write(client, bufor, n);
		//}
		
		//TODO
		//n = sprintf(bufor, "Cache-Control: no-store, no-cache, must-revalidate\n");
		//write(client, bufor, n);
		
		//not needed
		//n = sprintf(bufor, "Clear-Site-Data: "*"\n");
		//write(client, bufor, n);
		
		//cant be used with http2
		//n = sprintf(bufor, "Connection: Keep-Alive\n");
		//write(client, bufor, n);
		
		//n = sprintf(bufor, "Content-Disposition: attachment; filename="defaultName.html"\n");
		//write(client, bufor, n);
		
		//default
		//n = sprintf(bufor, "Content-Encoding: identity\n");
		//write(client, bufor, n);
		
		//TODO
		//n = sprintf(bufor, "Content-Length: 123\n");
		//write(client, bufor, n);
		
		//TODO
		//PUT Content-Location: ./new.html		
		
		//TODO
		//n = sprintf(bufor, ETag: "33a64df551425fcc55e4d42a148795d9f25f89d4\n");
		//write(client, bufor, n);
		
		//t->tm_year -= 1; 
		//strftime( godzina, 80, "%a, %d %b %Y %X GMT", t );
		//n = sprintf(bufor, "Expires: %s\n",godzina);
		//write(client, bufor, n);
		
		//TODO
		//n = sprintf(bufor, "Keep-Alive: timeout=15, max=100\n");
		//write(client, bufor, n);
		
		//TODO
		//date -r file.txt
		//n = sprintf(bufor, "Last-Modified: Wed, 21 Oct 2015 07:28:00 GMT\n");
		//write(client, bufor, n);
		
		//n = sprintf(bufor, "Set-Cookie: myCookie=BATMAN; Max-Age=60\n");
		//write(client, bufor, n);
		
		//default
		//n = sprintf(bufor, "Transfer-Encoding: identity\n");
		//write(client, bufor, n);

		//if(resNum==401){
		//	n = sprintf(bufor, "WWW-Authenticate: Basic\n");
		//	write(client, bufor, n);
		//}
		
		//default
		//n = sprintf(bufor, "X-DNS-Prefetch-Control: on\n");
		//write(client, bufor, n);
		
		//not needed
		//n = sprintf(bufor, "X-Frame-Options: deny\n");
		//write(client, bufor, n);
		
		//default
		//n = sprintf(bufor, "X-XSS-Protection: 1\n");
		//write(client, bufor, n);
		
		n = sprintf(bufor, "\n");
		write(client, bufor, n);
		
	}	
	if(method=="GET"){
		host=".";//TEMPORARY
		string wholePath=(host+"/"+fileName);
		thread t([client,wholePath]{
			char *threadBuffer = new char;  
			int res=open(wholePath.c_str(), O_RDONLY);
			if(res<0){
				printf("File error\n");
				printf("%s\n",wholePath.c_str());
				return 0;
			}
			while(read(res,threadBuffer,1)>0){
			    write(client, threadBuffer, 1);
				//write(1, threadBuffer, 1);
			}
			close(res);
		});
		//t.detach();	
		t.join();
	}
//}catch(...) resNum=500; //generic server error
	myClose(client);

    return 0;
}
